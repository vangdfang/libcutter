#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <optional>
#include <string>

class KeySet
{
public:
    KeySet(unsigned long key0, unsigned long key1, unsigned long key2, unsigned long key3)
        : key0(key0), key1(key1), key2(key2), key3(key3)
    {
        // no-op
    }

    unsigned long key0 = 0ul;
    unsigned long key1 = 0ul;
    unsigned long key2 = 0ul;
    unsigned long key3 = 0ul;
};

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

    KeySet moveKeys() const;
    KeySet lineKeys() const;
    KeySet curveKeys() const;

private:
    struct OptionalKeySet {
        std::optional<unsigned long> key0;
        std::optional<unsigned long> key1;
        std::optional<unsigned long> key2;
        std::optional<unsigned long> key3;
    };

    bool isKeySetComplete(const OptionalKeySet& keySet) const;
    bool isComplete() const;

    KeySet toKeySet(const OptionalKeySet& keySet) const;

    OptionalKeySet& getKeySetForKeyName(std::string keyName);
    std::optional<unsigned long>& getKeyForKeyName(OptionalKeySet& keySet, std::string keyName) const;

    OptionalKeySet m_moveKeys{};
    OptionalKeySet m_lineKeys{};
    OptionalKeySet m_curveKeys{};
};

#endif