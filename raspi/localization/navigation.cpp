#include <queue>

#include "navigation.h"

struct PriorityNode {
  Node<Point>* node;
  int priority;

  bool operator < (const PriorityNode& pn) {
    return priority < pn.priority;
  }
};

void Navigation::UpdateRoute() {
  // Current node on map.
  auto current_node = Location::GetCurrentNode();

  // Check if current point is in route cache.
  auto node_in_path_iter = find(current_route_.begin(), current_route_.end(),
      current_node);
  if (node_in_path_iter != current_route_.end()) {
    current_start_ = node_in_path_iter;
    return;
  }

  auto target_node = Map::GetTargetNode();

  unordered_map<Node<Point>*, Node<Point>*> came_from;
  unordered_map<Node<Point>*, int> cost_so_far;
  priority_queue<PriorityNode> frontier;

  frontier.push({ Location::GetCurrentNode(), 0 });
  came_from[current_node] = current_node;
  cost_so_far[current_node] = 0;

  while (not frontier.empty()) {
    // Current iteration node.
    auto current = frontier.pop();

    if (current.node == target_node) {
      break;
    }

    for (auto next : current.node->neighbors()) {
      int new_cost = cost_so_far[current.node] + next.distance;
      if (cost_so_far.count(next.node) == 0 ||
          new_cost < cost_so_far[next.node]) {
        cost_so_far[next.node] = new_cost;
        int priority = new_cost + heuristic(next.node, target_node);
        frontier.push({ next.node, priority });
        came_from[next.node] = current.node;
      }
    }
  }

  for (auto node = target_node
}
