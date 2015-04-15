#include <cassert>
#include <queue>
#include <stdexcept>

#include "global.h"
#include "location.h"
#include "map.h"
#include "navigation.h"

namespace wins {

using namespace std;

#define MULTIPLIER 10
#define NEIGHBOR_RADIUS 3

namespace {
  std::vector<std::string> &split(const std::string &s,
    char delim, std::vector<std::string> &elems) {
      std::stringstream ss(s);
      std::string item;
      while (std::getline(ss, item, delim)) {
          elems.push_back(item);
      }
      return elems;
  }


  std::vector<std::string> split(const std::string &s, char delim) {
      std::vector<std::string> elems;
      split(s, delim, elems);
      return elems;
  }
}

struct PriorityNode {
  kdtree::node<Point*>* node;
  double priority;
};

kdtree::node<Point*>* Navigation::destination_node_ = nullptr;
vector<kdtree::node<Point*>*>::reverse_iterator Navigation::current_start_;
vector<kdtree::node<Point*>*> Navigation::current_route_;
mutex Navigation::route_mutex;

bool Navigation::TrySetDestinationFromCoords(string s) {
  vector<string> coords = split(s, ',');
  double in_xd = stod(coords[0]);
  double in_yd = stod(coords[1]);
  kdtree::node<Point*>* n = Map::NodeNearest(stod(coords[0]), stod(coords[1]));
  int in_xi = (int)(in_xd * MULTIPLIER);
  int in_yi = (int)(in_yd * MULTIPLIER);
  int n_xi = (int)(n->point->x * MULTIPLIER);
  int n_yi = (int)(n->point->y * MULTIPLIER);

  if (in_xi == n_xi and in_yi == n_yi) {
    destination_node_ = n;
    return true;
  }
  return false;
}

void Navigation::ResetDestination() {
  destination_node_ = nullptr;
}

void Navigation::UpdateRoute() {
  if (destination_node_ == nullptr) {
    current_route_.clear();
    return;
  }

  // Current node on map.
  auto current_node = Location::GetCurrentNode();

  // Check if current point is in route cache.
  if (current_route_.size() > 0) {
    auto node_in_path_iter = find(current_start_, current_route_.rend(),
        current_node);
    if (node_in_path_iter != current_route_.rend()) {
      current_start_ = node_in_path_iter;
      return;
    }
  }

  auto target_node = destination_node_;
  assert(target_node != nullptr);

  auto compareFunc = [](PriorityNode a, PriorityNode b) {
    return a.priority > b.priority;
  };
  typedef priority_queue<PriorityNode, vector<PriorityNode>,
      decltype(compareFunc)> pqueue;

  unordered_map<kdtree::node<Point*>*, kdtree::node<Point*>*> came_from;
  unordered_map<kdtree::node<Point*>*, double> cost_so_far;
  pqueue frontier(compareFunc);

  frontier.push({ Location::GetCurrentNode(), 0 });
  came_from[current_node] = current_node;
  cost_so_far[current_node] = 0;

  while (not frontier.empty()) {
    // Current iteration node.
    auto current = frontier.top();
    frontier.pop();

    if (current.node == target_node) {
      break;
    }

    auto neighbors = Map::NodesInRadius(current.node,
        NEIGHBOR_RADIUS * Global::Scale);
    auto rem_iter = neighbors.begin();
    for (auto n = neighbors.begin(); n != neighbors.end(); ++n) {
      if (*n == current.node) {
        rem_iter = n;
        //cout << "Removed current\n";
      }
    }
    neighbors.erase(rem_iter);
    //printf("%3.0f %3.0f\n", current.node->point->x, current.node->point->y);
    for (size_t i = 0; i < neighbors.size(); ++i) {
      kdtree::node<Point*>* next = neighbors[i];
      double new_cost = cost_so_far[current.node] + current.node->distance(next);
      //printf("%3.0f %3.0f %5.3f", next->point->x, next->point->y, new_cost);
      if (cost_so_far.count(next) == 0 ||
          new_cost < cost_so_far[next]) {
        cost_so_far[next] = new_cost;
        double priority = new_cost + next->distance(target_node);
        //printf(" %5.3f", priority);
        frontier.push(PriorityNode{ next, priority });
        came_from[next] = current.node;
      }
      //cout << "\n";
    }
    //cout << "\n";
  }

  lock_guard<mutex> lock(route_mutex);
  current_route_.clear();
  for (auto node = target_node; node != current_node; node = came_from[node]) {
    current_route_.push_back(node);
  }
  current_route_.push_back(current_node);
  current_start_ = current_route_.rbegin();
}

vector<kdtree::node<Point*>*>::const_reverse_iterator Navigation::route_begin() {
  return current_start_;
}

vector<kdtree::node<Point*>*>::const_reverse_iterator Navigation::route_end() {
  return current_route_.rend();
}

}
