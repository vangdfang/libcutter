#include "KeyConfigParser.hpp"

#include <fstream>

namespace
{
    bool startsWith(const std::string& str, const std::string& needle)
    {
        // https://stackoverflow.com/questions/1878001/how-do-i-check-if-a-c-stdstring-starts-with-a-certain-string-and-convert-a
        return (str.rfind(needle, 0) == 0);
    }
} // namespace anonymous

KeyConfigParser::KeyConfigParser(const std::string& configFilePath)
{
    // Open the file!
    std::ifstream fileStream(configFilePath);

    if (!fileStream.is_open())
    {
        throw std::invalid_argument("Could not open config file");
    }

    while (fileStream.good())
    {
        // Formatted extraction pulls a string and a ulong from a string,
        // separated by whitespace.
        std::string keyName;
        unsigned long keyValue;
        fileStream >> keyName >> keyValue;

        auto keySet = getKeySetForKeyName(keyName);
    }
}

KeyConfigParser::KeySet& KeyConfigParser::getKeySetForKeyName(std::string keyName)
{
    if (startsWith(keyName, "MOVE_KEY"))
    {
        return m_moveKeys;
    }
    else if (startsWith(keyName, "LINE_KEY"))
    {
        return m_lineKeys;
    }
    else if (startsWith(keyName, "CURVE_KEY"))
    {
        return m_curveKeys;
    }
    else
    {
        throw std::invalid_argument(std::string("Unknown key: ").append(keyName));
    }
}