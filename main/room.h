#ifndef MUDOUC_ROOM_H
#define MUDOUC_ROOM_H
#pragma once
#include <string>
enum class RoomType {
    Start,
    Battle,
    Event,
    Shop,
    Rest,
    Boss
};
class Room {
public:
    Room(int id, std::string name, std::string description, RoomType type);
    int getId() const;
    RoomType getType() const;
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    void showInfo() const;

private:
    int id_;
    std::string name_;
    std::string description_;
    RoomType type_;
};

#endif //MUDOUC_ROOM_H
