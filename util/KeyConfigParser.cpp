#include "KeyConfigParser.hpp"

#include <fstream>

namespace
{
    // https://stackoverflow.com/questions/1878001/how-do-i-check-if-a-c-stdstring-starts-with-a-certain-string-and-convert-a
    bool startsWith(const std::string& str, const std::string& needle)
    {
        return (str.rfind(needle, 0) == 0);
    }

    // https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
    bool endsWith(const std::string& str, const std::string& suffix)
    {
        return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
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
        auto specificKey = getKeyForKeyName(keySet, keyName);
        specificKey = keyValue;
    }

    if (!isComplete())
    {
        throw std::invalid_argument("Incomplete configuration file. You need keys 0-3 for MOVE, LINE, and CURVE.");
    }
}

bool KeyConfigParser::isKeySetComplete(const KeyConfigParser::KeySet& keySet) const
{
    return keySet.key0.has_value() &&
        keySet.key1.has_value() &&
        keySet.key2.has_value() &&
        keySet.key3.has_value();
}

bool KeyConfigParser::isComplete() const
{
    return isKeySetComplete(m_moveKeys) &&
        isKeySetComplete(m_lineKeys) &&
        isKeySetComplete(m_curveKeys);
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

std::optional<unsigned long>& KeyConfigParser::getKeyForKeyName(KeyConfigParser::KeySet& keySet, std::string keyName) const
{
    if (endsWith(keyName, "0"))
    {
        return keySet.key0;
    }
    else if (endsWith(keyName, "1"))
    {
        return keySet.key1;
    }
    else if (endsWith(keyName, "2"))
    {
        return keySet.key2;
    }
    else if (endsWith(keyName, "3"))
    {
        return keySet.key3;
    }
    else
    {
        throw std::invalid_argument(std::string("Unknown key: ").append(keyName));
    }
}