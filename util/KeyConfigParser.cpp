#include "KeyConfigParser.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

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

    template<typename T>
    std::string toString(std::optional<T> opt)
    {
        if (opt.has_value())
        {
            std::stringstream ss;
            ss << *opt;
            return ss.str();
        }
        else
        {
            return "(missing)";
        }
    }
} // namespace anonymous

KeyConfigParser::KeyConfigParser(const std::string& configFilePath)
{
    // Open the file!
    std::ifstream fileStream(configFilePath);

    if (!fileStream.is_open())
    {
        throw std::invalid_argument("Could not open config file:" + configFilePath);
    }

    while (fileStream.good())
    {
        // Formatted extraction pulls two strings from a string, separated by
        // whitespace. The first should be the "name" of the key, followed by
        // the value it should have. We use `stoul` so that users can provide
        // the number in formats like `0x00ul`, `0ul`, and `0`.
        std::string keyName;
        std::string keyValue;
        fileStream >> keyName >> keyValue;

        auto& keySet = getKeySetForKeyName(keyName);
        auto& specificKey = getKeyForKeyName(keySet, keyName);
        const auto value = std::stoul(keyValue, nullptr /* idx */, 16 /* base */);
        specificKey = value;

        // Debug
        // std::cout << keyName << ": " << value << " (" << keyValue << ")" << std::endl;
    }

    if (!isComplete())
    {
        std::stringstream errorMessage;
        errorMessage << "Incomplete configuration file. You need keys 0-3 for MOVE, LINE, and CURVE." << std::endl;
        errorMessage << std::endl;
        errorMessage << "Move keys: " << m_moveKeys.str() << std::endl;
        errorMessage << "Line keys: " << m_lineKeys.str() << std::endl;
        errorMessage << "Curve keys: " << m_curveKeys.str() << std::endl;
        errorMessage << std::endl;
        throw std::invalid_argument(errorMessage.str());
    }
}

KeySet KeyConfigParser::moveKeys() const
{
    return toKeySet(m_moveKeys);
}

KeySet KeyConfigParser::lineKeys() const
{
    return toKeySet(m_lineKeys);
}

KeySet KeyConfigParser::curveKeys() const
{
    return toKeySet(m_curveKeys);
}

std::string KeyConfigParser::OptionalKeySet::str() const
{
    std::stringstream str;
    str << "[";
    str << toString(key0);
    str << ", ";
    str << toString(key1);
    str << ", ";
    str << toString(key2);
    str << ", ";
    str << toString(key3);
    str << "]";
    return str.str();
}

bool KeyConfigParser::isKeySetComplete(const KeyConfigParser::OptionalKeySet& keySet) const
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

KeySet KeyConfigParser::toKeySet(const KeyConfigParser::OptionalKeySet& keySet) const
{
    return KeySet(keySet.key0.value(),
        keySet.key1.value(),
        keySet.key2.value(),
        keySet.key3.value());
}

KeyConfigParser::OptionalKeySet& KeyConfigParser::getKeySetForKeyName(std::string keyName)
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

std::optional<individual_key_t>& KeyConfigParser::getKeyForKeyName(KeyConfigParser::OptionalKeySet& keySet, std::string keyName) const
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
