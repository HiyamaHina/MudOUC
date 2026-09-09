#include "Room.h"

#include <iostream>
#include <utility>

Room::Room(int id, std::string name, std::string description, RoomType type)
    : id_(id), name_(std::move(name)), description_(std::move(description)), type_(type) {}

int Room::getId() const {
    return id_;
}

RoomType Room::getType() const {
    return type_;
}

void Room::showInfo() const {
    std::cout << "\n========================================\n";
    std::cout << "[" << id_ << "/11] " << name_ << "\n";
    std::cout << description_ << "\n";
    std::cout << "========================================\n";
}
