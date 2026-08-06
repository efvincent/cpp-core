/**
 * @file core.cppm
 * @brief Aggregator module that re-exports the core foundational modules.
 */

/**
 * @module core
 * @brief Convenience import surface for downstream consumers.
 */
export module core;

// Layer 0: Primitives & portability
export import core_types;
export import core_portability;
export import core_option;
export import core_result;
export import core_algorithm;

// Layer 1: Memory & State
export import core_memory;
export import core_string;

// Layer 2: Utilities & Data Structures
export import core_hash;
export import core_hash_map;

// Layer 3: Parsing & I/O
export import core_text_parse;
export import core_lexer;
export import core_lexer_coord;
export import core_console;
export import core_file;