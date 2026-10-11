#include "NotificationManager.h"

namespace CMPUT350 {

void NotificationManager::Register(std::weak_ptr<GameObject> object, const std::string &key)
{
    std::shared_ptr<GameObject> gameObject = object.lock();
    if (gameObject == nullptr || !gameObject->IsAlive()){
        return;
    }
    auto &listeners = mListeners[key];
    for (auto it = listeners.begin(); it != listeners.end();){
        std::shared_ptr<GameObject> current = it->lock();
        if (current == nullptr || !current->IsAlive()){
            it = listeners.erase(it);
        }
        else
        {
            if (current == gameObject) {
                return;
            }
            ++it;
        }
    }

    listeners.push_back(object);
}

void NotificationManager::Unregister(std::weak_ptr<GameObject> object, const std::string &key)
{
    auto it = mListeners.find(key);
    if (it == mListeners.end()){
        return;
    }
    std::shared_ptr<GameObject> gameObject = object.lock();
    auto &listeners = it->second;
    for (auto listener = listeners.begin(); listener != listeners.end();) {
        std::shared_ptr<GameObject> current = listener->lock();
        if (current == nullptr || !current->IsAlive() || current == gameObject){
            listener = listeners.erase(listener);
        }
        else{
            ++listener;
        }
    }

    if (listeners.empty()){
        mListeners.erase(it);
    }
}

void NotificationManager::Notify(const std::string &message)
{
    auto it = mListeners.find(message);
    if (it == mListeners.end()){
        return;
    }
    std::vector<std::weak_ptr<GameObject>> listeners = it->second;
    for (auto &listener : listeners){
        std::shared_ptr<GameObject> gameObject = listener.lock();
        if (gameObject == nullptr || !gameObject->IsAlive()){
            continue;
        }
        auto current = mListeners.find(message);
        if (current == mListeners.end()){
            break;
        }
        bool registered = false;
        for (auto &item : current->second){
            if (item.lock() == gameObject){
                registered = true;
                break;
            }
        }
        if (registered){
            gameObject->ReceiveNotification(message);
        }
    }
    auto current = mListeners.find(message);
    if (current == mListeners.end()){
        return;
    }
    auto &registeredListeners = current->second;
    for (auto listener = registeredListeners.begin(); listener != registeredListeners.end();){
        std::shared_ptr<GameObject> gameObject = listener->lock();
        if (gameObject == nullptr || !gameObject->IsAlive()){
            listener = registeredListeners.erase(listener);
        }
        else{
            ++listener;
        }
    }
    if (registeredListeners.empty()){
        mListeners.erase(current);
    }
}
}  // namespace CMPUT350
