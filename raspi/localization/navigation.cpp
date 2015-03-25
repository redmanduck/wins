#include <cassert>
#include <queue>
#include <stdexcept>

#include "navigation.h"
#include "location.h"

using namespace std;

struct PriorityNode {
  kdtree::node<Point>* node;
  int priority;

  bool operator < (const PriorityNode& pn) const {
    return priority < pn.priority;
  }
};

int heuristic(kdtree::node<Point>* p1, kdtree::node<Point>* p2) {
  throw runtime_error("Not Implemented");
}

kdtree::node<Point>* Navigation::destination_node_ = nullptr;
vector<kdtree::node<Point>*>::reverse_iterator Navigation::current_start_;
vector<kdtree::node<Point>*> Navigation::current_route_;
mutex Navigation::route_mutex;

bool Navigation::TrySetDestinationFromCoords(string s) {

}

void Navigation::UpdateRoute() {
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

  unordered_map<kdtree::node<Point>*, kdtree::node<Point>*> came_from;
  unordered_map<kdtree::node<Point>*, int> cost_so_far;
  priority_queue<PriorityNode> frontier;

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

    auto neighbors = current.node->neighbors;
    for (size_t i = 0; i < neighbors.size(); ++i) {
      kdtree::node<Point>* next = neighbors[i];
      int new_cost = cost_so_far[current.node] + current.node->cost[i];
      if (cost_so_far.count(next) == 0 ||
          new_cost < cost_so_far[next]) {
        cost_so_far[next] = new_cost;
        int priority = new_cost + heuristic(next, target_node);
        frontier.push(PriorityNode{ next, priority });
        came_from[next] = current.node;
      }
    }
  }

  lock_guard<mutex> lock(route_mutex);
  current_route_.clear();
  for (auto node = target_node; node != current_node; node = came_from[node]) {
    current_route_.push_back(node);
  }
  current_route_.push_back(current_node);
  current_start_ = current_route_.rbegin();
}
