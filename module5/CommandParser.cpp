#include "CommandParser.h"
#include <exception>
#include <stdexcept>
#include <streambuf>
#ifdef _WIN32
#include <windows.h>
#endif

namespace module5 {
namespace {
// 同时显示操作输出并记住它，刷新界面后仍能看到上一回合的结果。
class OutputLog : public std::streambuf {
public:
    explicit OutputLog(std::streambuf* target) : target_(target) {}
    std::string text;
protected:
    int_type overflow(int_type ch) override {
        if (traits_type::eq_int_type(ch, traits_type::eof())) return traits_type::not_eof(ch);
        text += traits_type::to_char_type(ch);
        return target_->sputc(traits_type::to_char_type(ch));
    }
    std::streamsize xsputn(const char* data, std::streamsize count) override {
        text.append(data, static_cast<std::size_t>(count));
        return target_->sputn(data, count);
    }
    int sync() override { return target_->pubsync(); }
private:
    std::streambuf* target_;
};
}
int CommandParser::readChoice(std::istream& input, std::ostream& output, int min, int max) {
    std::string line;
    while (output << "请输入 " << min << " - " << max << "：" << std::flush,
           std::getline(input, line)) {
        const auto first = line.find_first_not_of(" \t\r\n");
        if (first != std::string::npos) {
            const auto value = line.substr(first, line.find_last_not_of(" \t\r\n") - first + 1);
            for (int i = min; i <= max; ++i)
                if (value == std::to_string(i)) return i;
        }
        output << "输入无效，请重新输入。\n";
    }
    throw std::runtime_error("输入已结束，取消当前选择。");
}
Action CommandParser::parse(const std::string& input) {
    const auto begin = input.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return Action::Invalid;
    const auto end = input.find_last_not_of(" \t\r\n");
    if (begin != end || input[begin] < '0' || input[begin] > '7')
        return Action::Invalid;
    return static_cast<Action>(input[begin] - '0');
}

ConsoleUI::ConsoleUI(std::istream& input, std::ostream& output, bool color)
    : input_(input), output_(output), color_(color) {}

void ConsoleUI::bind(Action action, std::function<void()> handler) {
    handlers_[action] = std::move(handler);
}

void ConsoleUI::menu() {
    if (color_) output_ << "\033[36m";
    output_ << "------------------------------------------------------------\n"
               "  1 攻击         2 查看背包       3 使用物品\n"
               "  4 前往下一房间 5 保存游戏       6 读取存档\n"
               "  7 详细状态     0 退出（不自动保存）\n"
               "============================================================\n";
    if (color_) output_ << "\033[0m";
}

void ConsoleUI::run() {
    std::string line;
    std::string lastResult;
    while (true) {
        if (color_) output_ << "\033[2J\033[H"; // 支持ANSI的终端刷新同一页
        if (header_) header_();
        menu();
        if (color_ && !lastResult.empty()) output_ << "[上次操作]\n" << lastResult << '\n';
        output_ << "请选择 > " << std::flush;
        if (!std::getline(input_, line)) break;
        const auto action = CommandParser::parse(line);
        if (action == Action::Exit) {
            output_ << "已退出游戏。\n";
            return;
        }
        if (action == Action::Invalid) {
            lastResult = "输入无效，请输入 0 到 7 中的一个数字，例如 1。\n";
            if (!color_) output_ << lastResult;
            continue;
        }
        const auto handler = handlers_.find(action);
        if (handler == handlers_.end() || !handler->second) {
            lastResult = "该动作尚未接入游戏模块。\n";
            if (!color_) output_ << lastResult;
            continue;
        }
        OutputLog log(output_.rdbuf());
        auto* original = output_.rdbuf(&log);
        try { handler->second(); }
        catch (const std::exception& error) {
            output_ << "操作失败：" << error.what() << '\n';
        }
        catch (...) {
            output_.rdbuf(original);
            throw;
        }
        output_.rdbuf(original);
        lastResult = log.text;
    }
    output_ << "\n输入已结束，退出游戏。\n";
}

bool prepareConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    return GetConsoleMode(handle, &mode) && SetConsoleMode(handle, mode | 0x0004);
#else
    return false;
#endif
}
}
