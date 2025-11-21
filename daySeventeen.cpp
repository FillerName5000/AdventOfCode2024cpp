#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Literal Operand Land // Combo Operand Cand
namespace DaySeventeen
{
	namespace {
		using instruction_set_t = std::vector<std::pair<int, int>>;
		void coutState();
		void coutCreateInstructionSet();
		enum class OpCode : int {
			Adv = 0, // A / Cand^2 truncated to int -> A
			Bxl = 1, // bitwise XOR B & Land -> B
			Bst = 2, // Cabd % 8 (aka only last three bits) -> B
			Jnz = 3, // nothing if A=0; else intstruction pointer jumps by Lan instead of two (check evenness)
			Bxc = 4, // botwise XOR B & C -> B, operand ignored
			Out = 5, // Cand % 8 -> output
			Bdv = 6, // A / Cand^2 truncated to int -> B
			Cdv = 7  // A / Cand^2 truncated to int -> C
		};

		const bool toLog = true; // true   false
		const int kMaxIter = 1'000'00;
#pragma region defs
		int regA = 0;
		int regB = 0;
		int regC = 0;
		instruction_set_t instructionSet;
		size_t instructionIndex = 0;
		std::string output = "";
#pragma endregion
		int getComboOperand(int operand)
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

		void executeInstruction(std::pair<int, int> instruction)
		{
			if (instruction.first < static_cast<int>(OpCode::Adv) || // best comparison to a < .size() call for cpp enum
				instruction.first > static_cast<int>(OpCode::Cdv)) {
				throw std::runtime_error("Invalid OpCode value");
			}

			OpCode opCode = static_cast<OpCode>(instruction.first);
			int operand = instruction.second;

			if (toLog) std::cout << "Operating: {" << instruction.first << ", " << instruction.second << "}\n";
			switch (opCode) {
			case OpCode::Adv: {
				int denA = 1 << getComboOperand(operand); // more efficient 2^int operation through left bitshift
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
				int result = getComboOperand(operand) % 8;
				output += std::to_string(result) + ","; // can only be single digit
				break;
			}
			case OpCode::Bdv: {
				int denominator = 1 << getComboOperand(operand); // more efficient 2^int operation through left bitshift
				if (denominator == 0) {
					throw std::runtime_error("Division by zero in Adv instruction");
				}
				regB = regA / denominator;
				break;
			}
			case OpCode::Cdv: {
				int denominator = 1 << getComboOperand(operand); // more efficient 2^int operation through left bitshift
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

		void parseInputLine(int i, std::string line)
		{
			switch (i) {
			case 1:
				regA = std::stoi(line.substr(line.find(":") + 1));
				return;
			case 2:
				regB = std::stoi(line.substr(line.find(":") + 1));
				return;
			case 3:
				regC = std::stoi(line.substr(line.find(":") + 1));
				return;
			case 4:
				return;
			case 5: {
				instructionSet.clear();
				size_t startPos = line.find(" ") + 1;
				line = line.substr(startPos);
				size_t pos = 0;
				while ((pos = line.find(",")) != std::string::npos) {
					int opCode = std::stoi(line.substr(0, pos));
					line.erase(0, pos + 1);
					pos = line.find(",");
					int operand = std::stoi(line.substr(0, pos));
					line.erase(0, pos + 1);
					instructionSet.emplace_back(opCode, operand);
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
				while (getline(inputFile, line)) {
					parseInputLine(i, line);
					++i;
				}

				std::cout << "Finished reading file\n";
				coutCreateInstructionSet();
				coutState();

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.

				std::cout << "Executing Assembly:\n";

				size_t s = instructionSet.size();
				int iter = 0;
				for (; instructionIndex < s; ++instructionIndex) {
					++iter;
					if (iter > kMaxIter) throw std::runtime_error("Inf loop");
					executeInstruction(instructionSet[instructionIndex]);
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
				<< ", Output: " << output << '\n';
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
	void daySeventeen() {
		std::system("cls"); // clear terminal pre-boot
		std::cout << "Running program DaySeventeen" << '\n';
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
		handleFile(inputFile);
	}
}
