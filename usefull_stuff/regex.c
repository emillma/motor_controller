#include <regex>
#include <string>
void handle_usb_input()
{

    static std::string data;
    bool any = false;
    const std::regex re("\xff([\x01-\xfd])([^]*?)(?=\xff[^\xfe])");
    const std::regex stuff("\xff\xfe");
    std::smatch match;
    std::string msg;

    for (int c = getchar_timeout_us(100); c != PICO_ERROR_TIMEOUT; c = getchar_timeout_us(100))
    {
        any = true;
        data += c;
    }
    if (!any)
        return;
    while (std::regex_search(data, match, re))
    {
        data = match.suffix().str();
        uint8_t type = match[1].str().c_str()[0];
        msg = std::regex_replace(match[2].str(), stuff, "\xff");
        if (type == 1) // watchdog
        {
            watchdog_update();
        }

        else if (type == 2) // Echo
        {
            usb_send_id(2);
            usb_send_stuffed((uint8_t *)msg.c_str(), msg.size());
            usb_flush();
        }
        else if (type == 3) // Stim
        {
            printf("to stim: %s", msg.c_str());
            stim_write((uint8_t *)msg.c_str(), msg.size());
        }
    }
}