#ifndef ENGINEVIEW_H
#define ENGINEVIEW_H

#include <memory>
#include <vector>
#include <string>

namespace CMPUT350 {

class GameObject;

class EngineView {
public:
    using tGameObject = std::vector<std::shared_ptr<GameObject>>;
    using const_iterator = tGameObject::const_iterator; //const such that galaga could access but couldn't make changes through iteration

    virtual void AddGameObject(std::shared_ptr<GameObject> gameObject) = 0;

    virtual void InstallKeyDownNotification(
        int key,
        const std::string& notification) = 0;

    virtual void InstallKeyUpNotification(
        int key,
        const std::string& notification) = 0;

    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
};

}  // namespace CMPUT350

#endif
