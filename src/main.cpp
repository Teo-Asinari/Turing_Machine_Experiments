#include <string>
#include <vector>
#include <sstream>
#include <iostream>

static const char *const BLANK_TAPE_SYMBOL = "ε";
static const char *const FINAL_TAPE_DELIMITER = "| ";
static const char *const RULE_WILDCARD_SYMBOL = "ANY";

static const bool g_debug = false;
using namespace std;

vector<string> splitString(string toSplit, char delim) {
    stringstream sstream(toSplit);
    string item;
    vector<string> splitted;
    while (getline(sstream, item, delim)) {
        splitted.push_back(item);
    }
    return splitted;
}

class TuringMachine {
    const char *const ruleNotFoundString = "RULE_NOT_FOUND";
    const char NOP_OPERATOR = 'N';
    const char OPERATOR_PRINT = 'P';
    const char OPERATOR_ERASE = 'E';
    const char OPERATOR_RIGHT = 'R';
    const char OPERATOR_LEFT = 'L';

    vector<string> tape;
    long int currTapeIdx = 0L;

    vector<string> m_configurations;
    string currMConfig;

    char rule_section_delim = '|';
    char operation_section_delim = ',';
    vector<string> rules;

    string getRule() {
        string ruleWithIdxPrefix = currMConfig + rule_section_delim +
                                   tape[currTapeIdx] + "," + to_string(currTapeIdx);
        string rulePrefix = currMConfig +
                            rule_section_delim + tape[currTapeIdx];
        string wildcardRulePrefix =
                currMConfig + rule_section_delim + RULE_WILDCARD_SYMBOL;

        string exactMatchWithIdxResult = "";
        string exactMatchResult = "";
        string negateOpMatchResult = "";
        string wildcardMatchResult = "";

        for (string rule: rules) {
            vector<string> ruleParts = splitString(rule, rule_section_delim);
            string mConfig = ruleParts[0];
            string symbol = ruleParts[1];
           auto prefixWithIdxResult = mismatch(ruleWithIdxPrefix.begin(), ruleWithIdxPrefix.end(),
                                                rule.begin());
            if (prefixWithIdxResult.first == ruleWithIdxPrefix.end()) {
                exactMatchWithIdxResult = rule;
                break;
            }
            auto prefixResult = mismatch(rulePrefix.begin(), rulePrefix.end(),
                                         rule.begin());
            if (prefixResult.first == rulePrefix.end() && symbol.find(",") ==
                                                               string::npos) {
                exactMatchResult = rule;
                break;
            }
            auto prefixWildcardResult = mismatch(wildcardRulePrefix.begin(),
                                                 wildcardRulePrefix.end(),
                                                 rule.begin());
            if (prefixWildcardResult.first == wildcardRulePrefix.end()) {
                wildcardMatchResult = rule;
            }
        }
        return !exactMatchWithIdxResult.empty() ? exactMatchWithIdxResult :
                !exactMatchResult.empty() ? exactMatchResult :
                !wildcardMatchResult.empty() ? wildcardMatchResult
                                            : ruleNotFoundString;
    }

    void parseAndExecuteRule(string rule) {
        vector<string> ruleParts = splitString(rule, rule_section_delim);
        vector<string> operations = splitString(ruleParts[2],
                                                operation_section_delim);
        string newMConfig = ruleParts[3];

        for (string operation : operations) {
            if (operation[0] == OPERATOR_PRINT) {
                if (g_debug) {
                    cout << "Printing: " << operation[1] << " at: "
                         << currTapeIdx
                         << "\n";
                }
                tape[currTapeIdx] = operation[1];
            } else if (operation[0] == OPERATOR_ERASE) {
                tape[currTapeIdx] = BLANK_TAPE_SYMBOL;
            } else if (operation[0] == OPERATOR_RIGHT) {
                if (g_debug) {
                    cout << "Going Right at: " << currTapeIdx <<
                         "\n";
                }
                if (currTapeIdx < tape.size()) {
                    currTapeIdx++;
                }
            } else if (operation[0] == OPERATOR_LEFT) {
                if (g_debug) {
                    cout << "Going Left at: " << currTapeIdx <<
                         "\n";
                }
                if (currTapeIdx > 0) {
                    currTapeIdx--;
                }
            } else if (operation[0] != NOP_OPERATOR) {
               if (g_debug) {
                   cout << "Weird operator: " << operation[0] << "\n";
               }
            }
        }

        if (g_debug) {
            cout << "Changing mConfig: " << currMConfig << " -> "
                 << newMConfig
                 << "\n";
        }
        currMConfig = newMConfig;
    }

    void performExecutionCycle() {
        if (g_debug) { cout <<
        "--------------------------------------------\n"; }
        string currRule = getRule();
        if (g_debug) {
            cout << "New cycle.\n";
            cout << "currRule is: " << currRule + "\n";
            cout << "currTapeIdx: " << currTapeIdx << "\n";
        }
        if (currRule != ruleNotFoundString) {
            parseAndExecuteRule(currRule);
        }
    }

public:
    TuringMachine(vector<string> new_tape,
                  vector<string> new_m_configs,
                  vector<string> new_rules) {
        tape = new_tape;
        m_configurations = new_m_configs;
        rules = new_rules;
    };

    void run(int maxIterations) {
        currMConfig = m_configurations[0];
        int numIterations = 0;
        while (numIterations < maxIterations) {
            performExecutionCycle();
            numIterations++;
        }
        ostringstream implodedTape;
        copy(tape.begin(), tape.end(),
             ostream_iterator<string>(implodedTape, FINAL_TAPE_DELIMITER));
        cout << implodedTape.str() + "\n";
    }
};

int main() {
    vector<string> simpleTMInitialTape(20);
    fill(simpleTMInitialTape.begin(), simpleTMInitialTape.end(),
         BLANK_TAPE_SYMBOL);
    TuringMachine simpleTM(simpleTMInitialTape,
                           vector<string>{"A", "B", "C", "D"},
                           vector<string>{"A|ε|P0,R|B",
                                          "B|ε|R|C",
                                          "C|ε|P1,R|D",
                                          "D|ε|R|A"});
    simpleTM.run(20);

    vector<string> simplifiedSimpleTMInitialTape(20);
    fill(simplifiedSimpleTMInitialTape.begin(),
         simplifiedSimpleTMInitialTape.end(), BLANK_TAPE_SYMBOL);
    TuringMachine simplifiedSimpleTM(simplifiedSimpleTMInitialTape,
                                     vector<string>{"B"},
                                     vector<string>{"B|ε|P0|B",
                                                    "B|0|R,R,P1|B",
                                                    "B|1|R,R,P0|B"});
    simplifiedSimpleTM.run(20);

    vector<string> bigSimpleTMInitialTape(100);
    fill(bigSimpleTMInitialTape.begin(), bigSimpleTMInitialTape.end(),
         BLANK_TAPE_SYMBOL);
    TuringMachine bigSimpleTM(bigSimpleTMInitialTape,
                              vector<string>{"A", "B", "C", "D"},
                              vector<string>{"A|ε|P0,R|B",
                                             "B|ε|R|C",
                                             "C|ε|P1,R|D",
                                             "D|ε|R|A"});
    bigSimpleTM.run(100);

    vector<string> singleOneTMInitialTape(20);
    fill(singleOneTMInitialTape.begin(), singleOneTMInitialTape.end(),
         BLANK_TAPE_SYMBOL);
    TuringMachine singleOneTM(singleOneTMInitialTape,
                              vector<string>{"B", "C", "D", "E", "F"},
                              vector<string>{"B|ε|P0,R|C",
                                             "C|ε|R|D",
                                             "D|ε|P1,R|E",
                                             "E|ε|R|F",
                                             "F|ε|P0,R|E"});
    singleOneTM.run(20);


    vector<string> transcendentalNumTMInitialTape(100);
    fill(transcendentalNumTMInitialTape.begin(),
         transcendentalNumTMInitialTape.end(), BLANK_TAPE_SYMBOL);
    TuringMachine transcendentalNumTM(transcendentalNumTMInitialTape,
                                      vector<string>{"B", "O", "Q", "P", "F"},
                                      vector<string>{
                                              "B|ε|Pə,R,Pə,R,P0,R,R,P0,L,L|O",
                                              "O|1|R,Px,L,L,L|O",
                                              "O|0||Q",
                                              "Q|1|R,R|Q",
                                              "Q|0|R,R|Q",
                                              "Q|ε|P1,L|P",
                                              "P|x|E,R|Q",
                                              "P|ə|R|F",
                                              "P|ε|L,L|P",
                                              "F|1|R,R|F",
                                              "F|0|R,R|F",
                                              "F|ε|P0,L,L|O"
                                      });
    transcendentalNumTM.run(100);

    vector<string> successiveIntsTMInitialTape(20);
    fill(successiveIntsTMInitialTape.begin(), successiveIntsTMInitialTape.end(),
         BLANK_TAPE_SYMBOL);
    TuringMachine successiveIntsTM(successiveIntsTMInitialTape,
                                   vector<string>{"BEGIN", "INCREMENT",
                                                  "REWIND1"},
                                   vector<string>{"BEGIN|ε|P0|INCREMENT",
                                                  "INCREMENT|0|P1|REWIND1",
                                                  "INCREMENT|1,0|N|REWIND2",
                                                  "INCREMENT|1|P0,L|INCREMENT",
                                                  "INCREMENT|ε|P1|REWIND1",
                                                  "REWIND1|ε|L|INCREMENT",
                                                  "REWIND1|0|R|REWIND1",
                                                  "REWIND1|1|R|REWIND1",
                                                  "REWIND2|ε|P1|INCREMENT",
                                                  "REWIND2|0|R|REWIND2",
                                                  "REWIND2|1|R|REWIND2",
                                   });
    successiveIntsTM.run(3100);
    return 0;
}
