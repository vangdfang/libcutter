#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <optional>
#include <string>

/**
 * Parse a "key config file"---a file containing the keys necessary to use the
 * cutter.
 *
 * This file is basically an INI file, but it's very simple:
 *
 * ```
 * MOVE_KEY_0 0123abcd
 * MOVE_KEY_1 0123abcd
 * MOVE_KEY_2 0123abcd
 * MOVE_KEY_3 0123abcd
 * LINE_KEY_0 0123abcd
 * LINE_KEY_1 0123abcd
 * LINE_KEY_2 0123abcd
 * LINE_KEY_3 0123abcd
 * CURVE_KEY_0 0123abcd
 * CURVE_KEY_1 0123abcd
 * CURVE_KEY_2 0123abcd
 * CURVE_KEY_3 0123abcd
 * ```
 */
class KeyConfigParser
{
public:
    KeyConfigParser(const std::string& configFilePath);

private:
    struct KeySet {
        std::optional<unsigned long> key0;
        std::optional<unsigned long> key1;
        std::optional<unsigned long> key2;
        std::optional<unsigned long> key3;
    };

    bool isKeySetComplete(const KeySet& keySet) const;
    bool isComplete() const;

    KeySet& getKeySetForKeyName(std::string keyName);

    KeySet m_moveKeys{};
    KeySet m_lineKeys{};
    KeySet m_curveKeys{};
};

#endif