#pragma once
#include <functional>
#include <istream>
#include <map>
#include <ostream>
#include <string>

namespace module5 {
// 数字与菜单一致：修改编号时，只改这里和 menu() 中的文字。
enum class Action { Invalid = -1, Exit = 0, Attack = 1, Bag = 2, Use = 3,
                    Move = 4, Save = 5, Load = 6, Look = 7 };

// 解析和执行分开：解析器不负责伤害计算、房间切换或文件格式。
class CommandParser {
public:
    static Action parse(const std::string& input);
    static int readChoice(std::istream& input, std::ostream& output, int min, int max);
};

class ConsoleUI {
public:
    ConsoleUI(std::istream& input, std::ostream& output, bool color = false);
    void bind(Action action, std::function<void()> handler);
    void setHeader(std::function<void()> header) { header_ = std::move(header); }
    void run();
    void menu();
private:
    std::istream& input_;
    std::ostream& output_;
    bool color_;
    std::function<void()> header_;
    std::map<Action, std::function<void()>> handlers_;
};
// Windows 开启 UTF-8；仅在终端支持时开启颜色，重定向输出使用纯文本。
bool prepareConsole();
}
