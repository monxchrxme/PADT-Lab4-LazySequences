#include "console_ui.hpp"

#include <iostream>
#include <limits>
#include <string>
#include <exception>

#include "../sequences/mutable_array_sequence.hpp"
#include "../sequences/immutable_array_sequence.hpp"
#include "../sequences/mutable_list_sequence.hpp"
#include "../sequences/immutable_list_sequence.hpp"
#include "../sequences/bit_sequence.hpp"
#include "../sequences/algorithms.hpp"
#include "../sequences/sequence_io.hpp"
#include "../types/pair.hpp"
#include "../types/option.hpp"

// ============================================================
// Global Types and Registries
// ============================================================
enum class SeqKind { MutArray, ImmArray, MutList, ImmList };

const char* seq_kind_name(SeqKind k) {
    switch (k) {
        case SeqKind::MutArray: return "Mutable Array";
        case SeqKind::ImmArray: return "Immutable Array";
        case SeqKind::MutList:  return "Mutable List";
        case SeqKind::ImmList:  return "Immutable List";
    }
    return "?";
}

bool is_mutable(SeqKind k) {
    return k == SeqKind::MutArray || k == SeqKind::MutList;
}

struct SeqEntry {
    SeqKind kind;
    Sequence<int> *ptr;
    ~SeqEntry() { delete ptr; }
};

struct BitEntry {
    BitSequence *ptr;
    ~BitEntry() { delete ptr; }
};

MutableArraySequence<SeqEntry*> seq_reg;
MutableArraySequence<BitEntry*> bit_reg;

// ============================================================
// Input/Output Utilities
// ============================================================
void clear_screen() {
    std::cout << "\x1B[2J\x1B[H" << std::flush;
}

void wait_for_enter() {
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.get();
}

int read_int(const char* prompt = "") {
    int v;
    while (true) {
        if (*prompt) std::cout << prompt;
        std::cin >> v;

        if (std::cin.eof()) {
            std::cout << "\n[!] Обнаружен конец потока (EOF). Завершение работы\n";
            exit(0);
        }

        if (!std::cin.fail()) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return v;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  [!] Ошибка. Введите целое число\n";
    }
}

int read_bit(const char* prompt = "") {
    while (true) {
        int v = read_int(prompt);
        if (v == 0 || v == 1) return v;
        std::cout << "  [!] Введите 0 или 1\n";
    }
}

// ============================================================
// Registry Management
// ============================================================
void print_seq_registry() {
    if (seq_reg.get_length() == 0) {
        std::cout << "  (пусто)\n";
        return;
    }
    for (int i = 0; i < seq_reg.get_length(); i++) {
        std::cout << "  " << (i+1) << ". "
                  << *(seq_reg[i]->ptr)
                  << " [" << seq_kind_name(seq_reg[i]->kind) << "]\n";
    }
}

void print_bit_registry() {
    if (bit_reg.get_length() == 0) {
        std::cout << "  (пусто)\n";
        return;
    }
    for (int i = 0; i < bit_reg.get_length(); i++) {
        std::cout << "  " << (i+1) << ". " << *(bit_reg[i]->ptr) << "\n";
    }
}

int pick_seq(const char* prompt = "  Номер: ") {
    while (true) {
        print_seq_registry();
        int n = read_int(prompt);
        if (n >= 1 && n <= seq_reg.get_length()) return n - 1;
        std::cout << "  [!] Номер вне диапазона\n";
    }
}

int pick_bit(const char* prompt = "  Номер: ") {
    while (true) {
        print_bit_registry();
        int n = read_int(prompt);
        if (n >= 1 && n <= bit_reg.get_length()) return n - 1;
        std::cout << "  [!] Номер вне диапазона\n";
    }
}

void add_seq(SeqKind kind, Sequence<int>* ptr) {
    seq_reg.append(new SeqEntry{kind, ptr});
    std::cout << "  -> Добавлено в реестр INT под номером " << seq_reg.get_length() << "\n";
}

void add_bit(BitSequence *ptr) {
    bit_reg.append(new BitEntry{ptr});
    std::cout << "  -> Добавлено в реестр BIT под номером " << bit_reg.get_length() << "\n";
}

void maybe_add_seq(SeqKind kind, Sequence<int>* result, Sequence<int>* original) {
    if (result != original) {
        std::cout << "  [i] Создан новый объект (Immutable или результат трансформации)\n";
        add_seq(kind, result);
    } else {
        std::cout << "  [i] Объект успешно изменён на месте (Mutable)\n";
    }
}

template <typename EntryType>
void remove_from_reg(MutableArraySequence<EntryType*>& reg, int idx) {
    delete reg[idx];
    reg.remove_at(idx);
    std::cout << "  -> Успешно удалено\n";
}

// ============================================================
// Map / Where / Reduce functions for int
// ============================================================
int map_double(const int& x) { return x * 2; }
int map_square(const int& x) { return x * x; }
bool where_positive(const int& x) { return x > 0; }
bool where_even(const int& x) { return x % 2 == 0; }
int reduce_sum(const int& a, const int& b) { return a + b; }
int reduce_max(const int& a, const int& b) { return a > b ? a : b; }

// set up a static state for the split condition (for lambda funcs)
static int g_split_val = 0;
bool split_condition(const int& x) { return x == g_split_val; }

// ============================================================
// Creating Objects
// ============================================================
MutableArraySequence<int>* read_int_array_temp() {
    int n;
    do { n = read_int("  Количество элементов: "); } while (n < 0);
    auto* s = new MutableArraySequence<int>();
    for (int i = 0; i < n; i++) {
        std::cout << "  [" << i << "]: ";
        s->append(read_int());
    }
    return s;
}

void cmd_create_seq() {
    std::cout << "  1. Mutable ArraySequence\n  2. Immutable ArraySequence\n"
              << "  3. Mutable ListSequence \n  4. Immutable ListSequence\n";
    int choice;
    do { choice = read_int("  > "); } while (choice < 1 || choice > 4);

    auto* tmp = read_int_array_temp();
    int sz = tmp->get_length();
    int* raw = new int[sz];
    for (int i = 0; i < sz; i++) {
        raw[i] = tmp->get(i);
    }
    delete tmp;

    switch (choice) {
        case 1: add_seq(SeqKind::MutArray, new MutableArraySequence<int>(raw, sz)); break;
        case 2: add_seq(SeqKind::ImmArray, new ImmutableArraySequence<int>(raw, sz)); break;
        case 3: add_seq(SeqKind::MutList,  new MutableListSequence<int>(raw, sz)); break;
        case 4: add_seq(SeqKind::ImmList,  new ImmutableListSequence<int>(raw, sz)); break;
    }
    delete[] raw;
}

void cmd_create_bit() {
    std::cout << "  1. Ввести побитово\n  2. Ввести из строки (например, 10101)\n";
    if (read_int("  > ") == 2) {
        std::string s;
        std::cout << "  Строка: ";
        std::cin >> s; std::cin.ignore();
        try { add_bit(new BitSequence(s)); }
        catch (const std::exception& e) { std::cout << "  [!] Ошибка: " << e.what() << "\n"; }
        return;
    }

    int n;
    do { n = read_int("  Количество бит: "); } while (n < 0);
    bool* bits = new bool[n];
    for (int i = 0; i < n; i++) {
        bits[i] = read_bit("  бит (0/1): ");
    }

    auto* bs = new BitSequence();
    for(int i = 0; i < n; ++i) {
        bs->append(Bit(bits[i]));
    }
    add_bit(bs);

    delete[] bits;
}

// ============================================================
// Operations Menu: INT Sequences (Decomposition)
// ============================================================
void cmd_seq_base_ops(int idx) {
    SeqEntry* e = seq_reg[idx]; Sequence<int>* s = e->ptr; SeqKind k = e->kind;
    std::cout << "\n  -- Базовые операции --\n"
              << "  1. append     2. prepend    3. insert_at\n"
              << "  4. remove_at  5. get (try_get)\n";
    try {
        switch (read_int("  > ")) {
            case 1: maybe_add_seq(k, s->append(read_int("  Значение: ")), s); break;
            case 2: maybe_add_seq(k, s->prepend(read_int("  Значение: ")), s); break;
            case 3: {
                int v = read_int("  Значение: "); int pos = read_int("  Индекс: ");
                maybe_add_seq(k, s->insert_at(v, pos), s);
                break;
            }
            case 4: maybe_add_seq(k, s->remove_at(read_int("  Индекс: ")), s); break;
            case 5: {
                Option<int> opt = s->try_get(read_int("  Индекс: "));
                if (opt.is_some()) std::cout << "  Значение: " << opt.unwrap() << "\n";
                else std::cout << "  [!] Индекс за пределами (None)\n";
                break;
            }
            default: std::cout << "  [!] Неверный выбор\n";
        }
    } catch (const std::exception& ex) { std::cout << "  [!] Ошибка: " << ex.what() << "\n"; }
}

void cmd_seq_transform_ops(int idx) {
    SeqEntry* e = seq_reg[idx];
    Sequence<int>* s = e->ptr;
    SeqKind k = e->kind;

    std::cout << "\n  -- Трансформации --\n"
              << "  1. get_subsequence    2. concat    3. slice    4. split\n";
    try {
        switch (read_int("  > ")) {
            case 1: {
                int from = read_int("  От: "); int to = read_int("  До: ");
                add_seq(k, s->get_subsequence(from, to));
                break;
            }
            case 2: {
                int other_idx = pick_seq("  Номер второй последовательности: ");
                add_seq(k, s->concat(seq_reg[other_idx]->ptr));
                break;
            }
            case 3: {
                int index = read_int("  Индекс старта: ");
                int count = read_int("  Кол-во удаляемых: ");
                std::cout << "  -- Ввод вставляемой последовательности --\n";
                auto* ins = read_int_array_temp();
                Sequence<int>* res = s->slice(index, count, ins);
                delete ins;
                maybe_add_seq(k, res, s);
                break;
            }
            case 4: {
                // Using global state to pass a parameter to a context-free function
                g_split_val = read_int("  Разделитель: ");
                auto* res = Split<int>(s, split_condition);

                std::cout << "  Фрагментов: " << res->get_length() << "\n";
                for (int i = 0; i < res->get_length(); i++) {
                    std::cout << "  [" << i+1 << "] " << *(*res)[i] << "\n";
                    seq_reg.append(new SeqEntry{SeqKind::MutArray, (*res)[i]});
                }
                delete res;
                break;
            }
            default: std::cout << "  [!] Неверный выбор\n";
        }
    } catch (const std::exception& ex) { std::cout << "  [!] Ошибка: " << ex.what() << "\n"; }
}

void cmd_seq_functional_ops(int idx) {
    SeqEntry* e = seq_reg[idx];
    Sequence<int>* s = e->ptr;
    SeqKind k = e->kind;

    std::cout << "\n  -- Map / Where / Reduce / Zip --\n"
              << "  1. map (x*2)      2. where (чётные)\n  3. reduce (сумма) 4. zip\n";
    try {
        switch (read_int("  > ")) {
            case 1: add_seq(k, Map<int, int>(s, map_double)); break;
            case 2: add_seq(k, s->where(where_even)); break;
            case 3: std::cout << "  Результат: " << s->reduce(reduce_sum, 0) << "\n"; break;
            case 4: {
                int other_idx = pick_seq("  Вторая последовательность для Zip: ");
                auto* res = Zip<int, int>(s, seq_reg[other_idx]->ptr);
                std::cout << "  Результат Zip: " << *res << "\n";
                std::cout << "  Выполнить Unzip? (1 - да): ";
                if (read_int() == 1) {
                    auto pair = Unzip<int, int>(res);
                    add_seq(SeqKind::MutArray, pair.first);
                    add_seq(SeqKind::MutArray, pair.second);
                }
                delete res;
                break;
            }
            default: std::cout << "  [!] Неверный выбор\n";
        }
    } catch (const std::exception& ex) { std::cout << "  [!] Ошибка: " << ex.what() << "\n"; }
}

void cmd_seq_menu(int idx) {
    while (true) {
        clear_screen();
        std::cout << "  Текущая: " << *(seq_reg[idx]->ptr) << "\n"
                  << "  Тип: " << seq_kind_name(seq_reg[idx]->kind) << "\n\n"
                  << "  1. Базовые операции\n  2. Трансформации\n"
                  << "  3. Функциональные операции (Map-Reduce)\n  0. Назад\n";
        switch (read_int("  > ")) {
            case 0: return;
            case 1: cmd_seq_base_ops(idx); wait_for_enter(); break;
            case 2: cmd_seq_transform_ops(idx); wait_for_enter(); break;
            case 3: cmd_seq_functional_ops(idx); wait_for_enter(); break;
            default: std::cout << "  [!] Неверная команда\n"; wait_for_enter();
        }
    }
}

// ============================================================
// Operations Menu: BitSequence
// ============================================================
void cmd_bit_ops(int idx) {
    BitSequence* b = bit_reg[idx]->ptr;

    std::cout << "\n  -- Битовые операции --\n"
              << "  1. AND   2. OR   3. XOR   4. NOT\n";
    try {
        int op = read_int("  > ");
        if (op == 4) { add_bit(b->bitwise_not()); return; }

        if (op >= 1 && op <= 3) {
            int other_idx = pick_bit("  Второй BitSequence: ");
            BitSequence* other = bit_reg[other_idx]->ptr;
            if (op == 1) add_bit(b->bitwise_and(other));
            if (op == 2) add_bit(b->bitwise_or(other));
            if (op == 3) add_bit(b->bitwise_xor(other));
        } else {
            std::cout << "  [!] Неверный выбор\n";
        }
    }
    catch (const std::exception& ex) {
        std::cout << "  [!] Ошибка: " << ex.what() << "\n";
    }
}

// ============================================================
// Main Entry Point
// ============================================================
void run_console_ui() {
    while (true) {
        clear_screen();
        std::cout << "  ====================================\n"
                  << "        INT Последовательности\n"
                  << "  ------------------------------------\n";
        print_seq_registry();
        std::cout << "\n        BitSequence\n"
                  << "  ------------------------------------\n";
        print_bit_registry();
        std::cout << "  ====================================\n"
                  << "  1. Создать INT последовательность\n"
                  << "  2. Создать BitSequence\n"
                  << "  3. Выбрать INT посл. для операций\n"
                  << "  4. Выбрать BitSequence для операций\n"
                  << "  5. Удалить INT последовательность\n"
                  << "  6. Удалить BitSequence\n"
                  << "  0. Выход\n"
                  << "  ====================================\n";

        switch (read_int("  > ")) {
            case 0:
                for (int i = 0; i < seq_reg.get_length(); i++) delete seq_reg[i];
                for (int i = 0; i < bit_reg.get_length(); i++) delete bit_reg[i];
                clear_screen();
                return;
            case 1: clear_screen(); cmd_create_seq(); wait_for_enter(); break;
            case 2: clear_screen(); cmd_create_bit(); wait_for_enter(); break;
            case 3:
                if (seq_reg.get_length() == 0) {
                    std::cout << "  [!] Пусто\n";
                    wait_for_enter();
                    break;
                }
                clear_screen();
                cmd_seq_menu(pick_seq("  Номер: "));
                break;
            case 4:
                if (bit_reg.get_length() == 0) {
                    std::cout << "  [!] Пусто\n";
                    wait_for_enter();
                    break;
                }
                clear_screen();
                cmd_bit_ops(pick_bit("  Номер: "));
                wait_for_enter();
                break;
            case 5:
                if (seq_reg.get_length() == 0) {
                    std::cout << "  [!] Пусто\n";
                    wait_for_enter();
                    break;
                }
                remove_from_reg(seq_reg, pick_seq("  Удалить номер: "));
                wait_for_enter();
                break;
            case 6:
                if (bit_reg.get_length() == 0) { std::cout << "  [!] Пусто\n";
                    wait_for_enter();
                    break;
                }
                remove_from_reg(bit_reg, pick_bit("  Удалить номер: "));
                wait_for_enter();
                break;
            default: std::cout << "  [!] Неверная команда\n"; wait_for_enter();
        }
    }
}