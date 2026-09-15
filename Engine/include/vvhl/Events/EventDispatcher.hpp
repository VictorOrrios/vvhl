#pragma once

#include <algorithm>
#include <typeindex>
#include <queue>
#include <vvhl/Core/Logger.hpp>
#include <vvhl/Core/UUID.hpp>
#include <vvhl/Events/Event.hpp>

namespace vvhl {

using ListenerID = UUID;

class EventDispatcher {
public:
  EventDispatcher() = default;
  ~EventDispatcher()  = default;

  void destroy(){ 
    m_listeners.clear(); 
    std::queue<std::function<void()>> empty;
    std::swap(m_eventQueue, empty);
  }

  template <typename EventType>
  ListenerID subscribe(std::function<void(const EventType &)> callback) {
    ListenerID id;

    auto wrapper = [callback](const Event &event) {
      callback(static_cast<const EventType &>(event));
    };

    m_listeners[typeid(EventType)].push_back({id, wrapper});

    return id;
  }

  template <typename EventType> void unsubscribe(ListenerID id) {

    auto it = m_listeners.find(typeid(EventType));

    if (it == m_listeners.end())
      return;

    auto &listeners = it->second;

    listeners.erase(
        std::remove_if(listeners.begin(), listeners.end(),
                       [&](const Listener &l) { return l.id == id; }),
        listeners.end());
  }

  template <typename EventType>
    requires std::derived_from<EventType, Event>
  void dispatch(const EventType &event) {
    auto it = m_listeners.find(typeid(EventType));

    if (it == m_listeners.end())
      return;

    for (const auto &listener : it->second)
      listener.callback(event);
  }

  template <typename EventType>
    requires std::derived_from<EventType, Event>
  void enqueue(const EventType &event) {
    m_eventQueue.push([this, event]() {
        dispatch<EventType>(event);
    });
  }

  void poll() {
    std::queue<std::function<void()>> toProcess;
    std::swap(toProcess, m_eventQueue);

    while (!toProcess.empty()) {
        toProcess.front()(); // Execute lamba (dispatch)
        toProcess.pop();
    }
  }


public:
  size_t pendingCount() const { return m_eventQueue.size(); }

private:
  struct Listener {
    ListenerID id;
    std::function<void(const Event &)> callback;
  };

  std::unordered_map<std::type_index, std::vector<Listener>> m_listeners;
  std::queue<std::function<void()>> m_eventQueue;
};

} // namespace vvhl