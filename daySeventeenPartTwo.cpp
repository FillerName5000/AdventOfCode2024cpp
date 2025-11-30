#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <thread>
#include <atomic>
#include <vector>

// Literal Operand Land // Combo Operand Cand
namespace DaySeventeenPartTwo
{
	namespace {
		using longg_t = long long;
		using instruction_set_t = std::vector<std::pair<longg_t, longg_t>>;
		void coutState();
		void coutCreateInstructionSet();
		enum class OpCode : longg_t {
			Adv = 0, // A / Cand^2 truncated to int -> A
			Bxl = 1, // bitwise XOR B & Land -> B
			Bst = 2, // Cabd % 8 (aka only last three bits) -> B
			Jnz = 3, // nothing if A=0; else intstruction pointer jumps by Lan instead of two (check evenness); ALWAYS A POINT TO ZERO, second to last input for output print must be 3 & last must be 0
			Bxc = 4, // botwise XOR B & C -> B, operand ignored
			Out = 5, // Cand % 8 -> output // regB in full, regA in test
			Bdv = 6, // A / Cand^2 truncated to int -> B
			Cdv = 7  // A / Cand^2 truncated to int -> C
		};
		// {2, 4}, {1, 5}, {7, 5}, {0, 3}, {4, 1}, {1, 6}, {5, 5}, {3, 0}
		/* B gets inited as regA l3B
		regB = regA l3b				{2,4}
		regB = regB ^ 101 (5)		{1,5}
		regC = regA / ((regB)^2)	{7,5}
		regA = regA / ((3)^2)		{0,3}
		regB = regB ^ regC			{4,1}
		regB = regB ^ 110 (6)		{1,6}
		OUT regB l3b				{5,5}
		KICK IT BACK NOW YALL
		-------------------------
		regB = (regA l3b) ^ 101
		regC = regA / ((regB)^2)
		regA = regA / 9
		regB = (regB ^ regC) ^ 110
		*/

		const bool toLog = false; // true   false
		const longg_t kMaxIter = 1000'000'000'000;
		//                       9'223'372'036'854'775'807
#pragma region defs
		longg_t regA = 0;
		longg_t regB = 0;
		longg_t regC = 0;
		instruction_set_t instructionSet;
		size_t instructionIndex = 0;
		std::string output = "";
#pragma endregion
		longg_t getComboOperand(longg_t operand)
		{
			switch (operand) {
			case 0:
			case 1:
			case 2:
			case 3:
				return operand;
			case 4:
				return regA;
			case 5:
				return regB;
			case 6:
				return regC;
			default:
				std::cout << "\nInvalid combo operand value:" << operand << '\n';
				throw std::runtime_error("Invalid combo operand value");
			}
		}
		void coutInstructionSetBitValues() {
			for (const auto& instruction : instructionSet) {
				std::cout << instruction.first << ": " << std::bitset<3>(instruction.first)
					<< ", " << instruction.second << ": " << std::bitset<3>(instruction.second) << " ";
			}
		}
		void executeInstruction(const std::pair<longg_t, longg_t>& instruction, std::vector<longg_t>& currentOutput, bool& hasAddedToOutput, size_t& addedIndex)
		{
			if (instruction.first < static_cast<longg_t>(OpCode::Adv) || // best comparison to a < .size() call for cpp enum
				instruction.first > static_cast<longg_t>(OpCode::Cdv)) {
				throw std::runtime_error("Invalid OpCode value");
			}

			OpCode opCode = static_cast<OpCode>(instruction.first);
			longg_t operand = instruction.second;

			if (toLog) std::cout << "\nOperating: {" << instruction.first << ", " << instruction.second << "}\n";
			switch (opCode) {
			case OpCode::Adv: {
				longg_t denA = static_cast<longg_t>(1) << getComboOperand(operand); // more efficient 2^int operation through left bitshift
				if (denA == 0) {
					throw std::runtime_error("Division by zero in Adv instruction");
				}
				regA = regA / denA;
				break;
			}
			case OpCode::Bxl:
				regB = regB ^ operand; // bitwise XOR doesn't care about leading false bits so default int is fine
				break;
			case OpCode::Bst:
				regB = getComboOperand(operand) % 8;
				break;
			case OpCode::Jnz:
				if (regA != 0) {
					if (operand % 2 != 0) {
						std::cout << "\nOperand for Jnz must be even:" << operand << '\n';
						throw std::runtime_error("Operand for Jnz must be even");
					}
					instructionIndex = (static_cast<size_t>(operand / 2)) - 1; // can underflow, will immediately overflow in the fori
					break;
				}
			case OpCode::Bxc:
				regB = regB ^ regC;
				break;
			case OpCode::Out: {
				longg_t outDigit = getComboOperand(operand) % 8;
				currentOutput.push_back(outDigit); // can only be three bit val
				hasAddedToOutput = true;
				++addedIndex;
				break;
			}
			case OpCode::Bdv: {
				longg_t denominator = static_cast<longg_t>(1) << getComboOperand(operand); // more efficient 2^int operation through left bitshift
				if (denominator == 0) {
					throw std::runtime_error("Division by zero in Adv instruction");
				}
				regB = regA / denominator;
				break;
			}
			case OpCode::Cdv: {
				longg_t denominator = static_cast<longg_t>(1) << getComboOperand(operand); // more efficient 2^int operation through left bitshift
				if (denominator == 0) {
					throw std::runtime_error("Division by zero in Adv instruction");
				}
				regC = regA / denominator;
				break;
			}
			default:
				throw std::runtime_error("Unhandled OpCode");
			}
			if (toLog) coutState();
		}

		void parseInputLine(int i, std::string line, std::vector<longg_t>& targetOutput)
		{
			switch (i) {
			case 1:
				regA = std::stoll(line.substr(line.find(":") + 1));
				return;
			case 2:
				regB = std::stoll(line.substr(line.find(":") + 1));
				return;
			case 3:
				regC = std::stoll(line.substr(line.find(":") + 1));
				return;
			case 4:
				return;
			case 5: {
				instructionSet.clear();
				size_t startPos = line.find(" ") + 1;
				line = line.substr(startPos);
				size_t pos = 0;
				while ((pos = line.find(",")) != std::string::npos) {
					longg_t opCode = std::stoll(line.substr(0, pos));
					line.erase(0, pos + 1);
					pos = line.find(",");
					longg_t operand = std::stoll(line.substr(0, pos));
					line.erase(0, pos + 1);
					instructionSet.emplace_back(opCode, operand);

					targetOutput.push_back(opCode);
					targetOutput.push_back(operand);
				}
				return;
			}
			default:
				std::cout << '\n' << line << '\n';
				throw std::runtime_error("line parsing error");
			}
		}

		void handleFile(std::ifstream& inputFile)
		{
			if (inputFile.is_open()) {
				std::string line;
				int i = 1;
				std::vector<longg_t> targetOutput;
				while (getline(inputFile, line)) {
					parseInputLine(i, line, targetOutput);
					++i;
				}

				std::cout << "Finished reading file\n";
				coutCreateInstructionSet();
				coutState();
				coutInstructionSetBitValues();

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.

				std::cout << "Executing Assembly Instructions:\n\n";

				size_t maxIndex = 1;

				size_t s = instructionSet.size();
				size_t bigS = targetOutput.size();
				longg_t iter = 0;
				longg_t regAStartValueToCheck = 0;
				bool hasAddedToOutput = false;
				bool baseValueHasBeenSet = false;

				size_t addedIndex = -1;
				longg_t baseValue = -1;
				std::vector<longg_t> currentOutput;
				try {
					while (iter < kMaxIter) {
						++iter;

						if (baseValue != -1) {
							regAStartValueToCheck += (1LL << 24);
							regAStartValueToCheck = (baseValue & 0xFFFFFF) | (regAStartValueToCheck & ~0xFFFFFF); // rightmost 24 bits as base
						}
						else {
							++regAStartValueToCheck;
						}

						regA = regAStartValueToCheck;
						currentOutput.clear();
						instructionIndex = 0;
						hasAddedToOutput = false;
						addedIndex = -1;

						for (; instructionIndex < s; ++instructionIndex) {
							executeInstruction(instructionSet[instructionIndex], currentOutput, hasAddedToOutput, addedIndex);

							if (hasAddedToOutput) {
								if (addedIndex > maxIndex) {
									maxIndex = addedIndex;
									std::cout << "Max index: " << addedIndex << '\n';
								}
								if (currentOutput[addedIndex] != targetOutput[addedIndex]) break;
								if (addedIndex == 15) {
									std::cout << "15\n";
									break;
								}
								if (addedIndex > 7 && addedIndex % 2 == 1) { // 4 pairs achieved (index)
									if (toLog)
									{
										std::cout << "Start value: " << regAStartValueToCheck - 1
											<< ", Binary: " << std::bitset<48>(regAStartValueToCheck - 1)
											<< ", Index: " << addedIndex
											<< " Output: ";
										for (const auto& val : currentOutput) {
											std::cout << val << " ";
										}
										std::cout << ";\n";
									}
								}

								if (!baseValueHasBeenSet && addedIndex == 7) {
									baseValue = regAStartValueToCheck;
									baseValueHasBeenSet = true;
									std::cout << "Base value set to: " << baseValue << '\n';
								}

								if (addedIndex == 13) {
									std::cout << "13: currentOutput: ";
									for (const auto& val : currentOutput) {
										std::cout << val << " ";
									}
									std::cout << '\n';

									std::cout << "13: targetOutput:  ";
									for (const auto& val : targetOutput) {
										std::cout << val << " ";
									}

									std::cout << '\n';
								}
								hasAddedToOutput = false;
							}
						}

						if (addedIndex == bigS - 1 && currentOutput[addedIndex] == targetOutput[addedIndex]) {
							std::cout << "\nFOUND\n";
							std::cout << "Addedindex: " << addedIndex << '\n';

							std::cout << "Found matching regAStartValueToCheck: " << regAStartValueToCheck << '\n'
								<< "Bitwise: " << std::bitset<64>(regAStartValueToCheck) << '\n';

							std::cout << "currentOutput: ";
							for (const auto& val : currentOutput) {
								std::cout << val << " ";
							}
							std::cout << '\n';

							std::cout << "targetOutput:  ";
							for (const auto& val : targetOutput) {
								std::cout << val << " ";
							}
							std::cout << '\n';

							return;
						}
					}
				}
				catch (const std::exception& e) {
					std::cerr << "Exception caught: " << e.what() << '\n';
					std::cerr << "Dumping state:\n";
					std::cerr << "iter: " << iter << '\n';
					std::cerr << "regAStartValueToCheck: " << regAStartValueToCheck << '\n';
					std::cerr << "regA: " << regA << '\n';
					std::cerr << "regB: " << regB << '\n';
					std::cerr << "regC: " << regC << '\n';
					std::cerr << "instructionIndex: " << instructionIndex << '\n';
					std::cerr << "currentOutput: ";
					for (const auto& val : currentOutput) {
						std::cerr << val << " ";
					}
					std::cerr << '\n';
					std::cerr << "targetOutput: ";
					for (const auto& val : targetOutput) {
						std::cerr << val << " ";
					}
					std::cerr << '\n';
					throw;
				}

				std::cout << "Output:\n" << output << '\n';

				std::cout << "\nFinished running program";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}
		void coutState()
		{
			std::cout << "Register A: " << regA
				<< ", Register B: " << regB
				<< ", Register C: " << regC
				<< ", Output: " << output << '\n'
				<< "Bits A: " << std::bitset<32>(regA) << '\n'
				<< "Bits B: " << std::bitset<32>(regB) << '\n'
				<< "Bits C: " << std::bitset<32>(regC) << '\n';

		}
		void coutCreateInstructionSet()
		{
			for (const auto& instruction : instructionSet) {
				std::cout << "{" << instruction.first
					<< ", " << instruction.second << "}, ";
			}
			std::cout << '\n';
		}
	}
	void daySeventeenPartTwo() {
		std::system("cls"); // clear terminal pre-boot
		std::cout << "Running program DaySeventeenPartTwo" << '\n';
		const bool isTestFile = false; // true   false

		std::string line;
		std::ifstream inputFile;
		if (isTestFile) {
			std::cout << "Using test file\n\n";
		}
		else {
			std::cout << "Using full file\n\n";
		}
		(isTestFile) ? inputFile.open("daySeventeenTest.txt") : inputFile.open("daySeventeenFull.txt");
		std::cout << "Size of long: " << sizeof(long) << " bytes\n";
		std::cout << "Maximum value of long: " << LONG_MAX << '\n';
		std::cout << "Minimum value of long: " << LONG_MIN << '\n';
		std::cout << "Size of long: " << sizeof(long long) << " bytes\n";
		std::cout << "Maximum value of long: " << LLONG_MAX << '\n';
		std::cout << "Minimum value of long: " << LLONG_MIN << '\n';
		handleFile(inputFile);
	}
}
// 11114980089195 too low 
// 88919840713560
// 130714697192208
// 16339337149026
// 
// 38651278428569

// 109020022606234 -> valid mirror but incorrect
/*
Start value: 0003287449, Binary: 0000000000001100|100010|100110|011001, Index: 5 Output: 2 4 1 5 7 5 ;
Start value: 0003385753, Binary: 0000000000001100|111010|100110|011001, Index: 5 Output: 2 4 1 5 7 5 ;
Start value: 0003705241, Binary: 0000000000001110|001000|100110|011001, Index: 5 Output: 2 4 1 5 7 5 ;
Start value: 0003706624, Binary: 0000000000001110|001000|111100|000000, Index: 5 Output: 2 4 1 5 7 5 ;
Start value: 0003706632, Binary: 0000000000001110|001000|111100|001000, Index: 5 Output: 2 4 1 5 7 5 ;
Start value: 0003732232, Binary: 0000000000001110|001111|001100|001000, Index: 5 Output: 2 4 1 5 7 5 ;
Start value: 0003978705, Binary: 0000000000001111|001011|010111|010001, Index: 5 Output: 2 4 1 5 7 5 ;
Start value: 0003994376, Binary: 0000000000001111|001111|001100|001000, Index: 5 Output: 2 4 1 5 7 5 ;
Start value: 0003995089, Binary: 0000000000001111|001111|010111|010001, Index: 5 Output: 2 4 1 5 7 5 ;

Start value: 0012093849, Binary: 000000000|101110|001000|100110|011001, Index: 7 Output: 2 4 1 5 7 5 0 3 ;
Start value: 0012095232, Binary: 000000000|101110|001000|111100|000000, Index: 7 Output: 2 4 1 5 7 5 0 3 ;
Start value: 0012095240, Binary: 000000000|101110|001000|111100|001000, Index: 7 Output: 2 4 1 5 7 5 0 3 ;
Start value: 0012120840, Binary: 000000000|101110|001111|001100|001000, Index: 7 Output: 2 4 1 5 7 5 0 3 ;
Start value: 0012159385, Binary: 000000000|101110|011000|100110|011001, Index: 7 Output: 2 4 1 5 7 5 0 3 ;
Start value: 0012160768, Binary: 000000000|101110|011000|111100|000000, Index: 7 Output: 2 4 1 5 7 5 0 3 ;
Start value: 0012160776, Binary: 000000000|101110|011000|111100|001000, Index: 7 Output: 2 4 1 5 7 5 0 3 ;

Start value: 1639483801, Binary: 001100001|101110|001000|100110|011001
Start value: 1639485192, Binary: 001100001|101110|001000|111100|001000
Start value: 1639485184, Binary: 001100001|101110|001000|111100|000000
Start value: 1639510792, Binary: 001100001|101110|001111|001100|001000
Start value: 1639549337, Binary: 001100001|101110|011000|100110|011001
Start value: 1639550720, Binary: 001100001|101110|011000|111100|000000
Start value: 1639550728, Binary: 001100001|101110|011000|111100|001000

Start value: 1773701529, Binary: 001101001|101110|001000|100110|011001
Start value: 1773702912, Binary: 001101001|101110|001000|111100|000000
Start value: 1773702920, Binary: 001101001|101110|001000|111100|001000
Start value: 1773728520, Binary: 001101001|101110|001111|001100|001000
Start value: 1773767065, Binary: 001101001|101110|011000|100110|011001
Start value: 1773768448, Binary: 001101001|101110|011000|111100|000000
Start value: 1773768456, Binary: 001101001|101110|011000|111100|001000
*/