// COMMENTED OUT CODE DISCLAIMER
// To ensure compilation for the other files with the BigInt.hpp or any other external header files, this entire file is commented out. 
// In the current structure that only uses one file at a time, header files are not perfectly organised.
// Having many files in a project you have no intention of using is not a normal use case, therefore organising this perfectly is not a priority.
// To test this file, get the BigInt hpp file from github and add it to the Header Files, in my case in a "libs" filter. 
// Comment out any other cpp files using it. Only the .cpp files should need to be edited.
// /COMMENTED OUT CODE DISCLAIMER
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <unordered_set> // not writing extension methods for std::pair hashing given the context of the project & accepting the performance hit of not using unordered_set
#include <vector>
#include "BigInt.hpp" // https://github.com/faheel/BigInt

namespace DayFourteenPartTwo
{
	// [0,0]
	//       [xLen-1,yLen-1]
	namespace {
		using coord_t = std::pair<BigInt, BigInt>;
	}
}

namespace std {
	template<>
	struct hash<DayFourteenPartTwo::coord_t> {
		size_t operator()(const DayFourteenPartTwo::coord_t& coord) const {
			// Get string representations of the BigInts
			string xStr = coord.first.to_string();
			string yStr = coord.second.to_string();

			// Combine the hashes using FNV-1a hash
			size_t h1 = hash<string>{}(xStr);
			size_t h2 = hash<string>{}(yStr);
			return h1 ^ (h2 << 1); // Simple hash combination
		}
	};
}

namespace DayFourteenPartTwo
{
	// [0,0]
	//       [xLen-1,yLen-1]
	namespace {
		struct CoordState {
			BigInt px, py, vx, vy;
		};

		void handleLine(std::string line);
		void coutSetCoordsAsGrid(const std::vector<std::unordered_set<coord_t>>& coordSets, BigInt xLen, BigInt yLen, int iters);
		void calcEndCoord(CoordState& coordState, const BigInt xLen, const BigInt yLen);
		BigInt calcEndDirCoord(BigInt pDir, BigInt vDir, BigInt dirLen);
		bool isIsolatedCoord(coord_t endCoord, std::unordered_set<coord_t> endCoords, BigInt xLen, BigInt yLen);

		const bool toLog = true; // true   false
		const BigInt kXLenFull = 101; // Using Bigint for everything to avoid any conversion errors in case it is used with other BigInts
		const BigInt kXLenTest = 11; // performance gains from using smaller int value types on most modern system is negligable
		const BigInt kYLenFull = 103;
		const BigInt kYLenTest = 7;
		int iterations = 0; // part two is going to be a quadrillion iterations
		int kMaxIter = 100000;

		std::vector<CoordState> coordStates;

		void printIterations(BigInt xLen, BigInt yLen) {
			std::vector<std::unordered_set<coord_t>> coordSets;
			std::unordered_set<coord_t> endCoords;
			if (toLog) std::cout << "\nStarting iteration calculations with dimensions " << xLen << "x" << yLen << "\n";

			while (iterations < kMaxIter)
			{
				++iterations;
				endCoords.clear();
				bool hasDuplicates = false;

				for (CoordState& coordState : coordStates)
				{
					calcEndCoord(coordState, xLen, yLen);
					if (!endCoords.insert(std::make_pair(coordState.px, coordState.py)).second) {
						hasDuplicates = true;
					//	if (toLog) std::cout << coordState.px << "-" << coordState.py;
						break;
					}
				}
				if (!hasDuplicates) {
					coordSets.push_back(endCoords);

					if (coordSets.size() == 1)
					{
						coutSetCoordsAsGrid(coordSets, xLen, yLen, iterations);
						coordSets.clear();
						std::cout << "\n\nI" << iterations << " ";
					}
				}
				if (iterations % 50 == 0) std::cout << "i" << iterations << " ";
			}

			if (!coordSets.empty())
			{
				coutSetCoordsAsGrid(coordSets, xLen, yLen, iterations);
			}

			if (toLog && iterations >= kMaxIter) {
				std::cout << "\nReached maximum iterations (" << kMaxIter << ") without finding solution\n";
			}
		}

		void handleFile(std::ifstream& inputFile, BigInt xLen, BigInt yLen)
		{
			if (inputFile.is_open()) {
				std::string line;
				while (getline(inputFile, line)) {
					handleLine(line);
				}

				std::cout << "Finished reading file.\nCalcing iterations...\n";

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.

				printIterations(xLen, yLen);

				std::cout << "Iterations: " << iterations << '\n';

				std::cout << "\nFinished running program";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}

		void handleLine(std::string line)
		{
			BigInt px, py, vx, vy;

			std::regex pattern(R"(p=(-?\d{1,3}),(-?\d{1,3})\s+v=(-?\d{1,3}),(-?\d{1,3}))");
			std::smatch matches;

			if (std::regex_match(line, matches, pattern)) {
				px = std::stoi(matches[1]);
				py = std::stoi(matches[2]);
				vx = std::stoi(matches[3]);
				vy = std::stoi(matches[4]);

				coordStates.push_back({ px, py, vx, vy });
			}
			else {
				std::cout << line << '\n';
				throw std::runtime_error("Failed to parse");
			}
		}


		BigInt calcEndDirCoord(const BigInt pDir, const  BigInt vDir, const  BigInt dirLen)
		{
			BigInt endState = pDir + (vDir * iterations);
			return ((endState % dirLen) + dirLen) % dirLen;
		}

		void calcEndCoord(CoordState& coordState, const BigInt xLen, const BigInt yLen)
		{
			BigInt endXCoord = calcEndDirCoord(coordState.px, coordState.vx, xLen);
			BigInt endYCoord = calcEndDirCoord(coordState.py, coordState.vy, yLen);

			coordState.px = endXCoord;
			coordState.vx = endYCoord;
		}

		bool isIsolatedCoord(coord_t endCoord, std::unordered_set<coord_t> endCoords, BigInt xLen, BigInt yLen)
		{
			// AI/LLM disclaimer: this was obviously generated. I've not restricted myself within this project to using it for:
			// 1: string parsing, logging, fundamental coordinate movements and general algorithms that are not complicated 
			//    or part of the challenge, but very vulnerable to e.g. off-by-one-errors with manual writing.
			// 2: writing explictly laid out common algorithms (stack, recursion, sorting) 
			//    with multi-sentence specifications for exit conditions, branching, and anything else specific to the challenge.
			// 3: Questions regarding c++ (that will be double-checked).
			// Queries are generally limited to the contents of a single function. 
			// The only time LLMs will read the original challenge text is to generate more test cases.
			// Vague "Fix ..." questions are not allowed.
			BigInt x = endCoord.first;
			BigInt y = endCoord.second;

			if (x == 0 || y == 0 || x == (xLen - 1) || x == (yLen - 1)) return false;

			coord_t neighbor;

			// North
			neighbor = std::make_pair(x, --y); ++y;
			if (endCoords.find(neighbor) != endCoords.end()) return false;

			// East
			neighbor = std::make_pair(++x, y); --x;
			if (endCoords.find(neighbor) != endCoords.end()) return false;

			// South
			neighbor = std::make_pair(x, ++y); --y;
			if (endCoords.find(neighbor) != endCoords.end()) return false;

			// West
			neighbor = std::make_pair(--x, y); ++x;
			if (endCoords.find(neighbor) != endCoords.end()) return false;

			// Northwest
			neighbor = std::make_pair(--x, --y); ++x; ++y;
			if (endCoords.find(neighbor) != endCoords.end()) return false;

			// Northeast
			neighbor = std::make_pair(++x, --y); --x; ++y;
			if (endCoords.find(neighbor) != endCoords.end()) return false;

			// Southwest
			neighbor = std::make_pair(--x, ++y); ++x; --y;
			if (endCoords.find(neighbor) != endCoords.end()) return false;

			// Southeast
			neighbor = std::make_pair(++x, ++y); --x; --y;
			if (endCoords.find(neighbor) != endCoords.end()) return false;

			return true;
		}

		void coutSetCoordsAsGrid(const std::vector<std::unordered_set<coord_t>>& coordSets, BigInt xLen, BigInt yLen, int iters)
		{
			std::ofstream outputFile("dayFourteenOutput.txt", std::ios::app);
			if (!outputFile.is_open()) {
				std::cout << "Failed to open output file\n";
				return;
			}

			std::string iterStr = std::to_string(iters);
			iterStr = std::string(7 - std::min(7ULL, iterStr.length()), '0') + iterStr.substr(0, 7);

			std::vector<std::vector<std::vector<int>>> grids;
			for (const auto& coords : coordSets)
			{
				std::vector<std::vector<int>> grid(yLen.to_int(), std::vector<int>(xLen.to_int(), 0));
				for (const auto& coord : coords)
				{
					grid[coord.second.to_int()][coord.first.to_int()]++;
				}
				grids.push_back(grid);
			}

			for (int y = 0; y < yLen.to_int(); y++)
			{
				for (size_t g = 0; g < grids.size(); g++)
				{
					for (int x = 0; x < xLen.to_int(); x++)
					{
						if (x < 7 && y == 0)
						{
							outputFile << iterStr[x];
							continue;
						}
						if (grids[g][y][x] == 0)
							outputFile << '.';
						else
							outputFile << grids[g][y][x];
					}
					if (g < grids.size() - 1)
						outputFile << "| ";
				}
				outputFile << '\n';
			}

			int totalWidth = (xLen.to_int() * grids.size()) + (grids.size() * 2 - 2);
			std::string dashes(totalWidth, '-');
			outputFile << dashes << '\n' << dashes << "\n\n";

			outputFile.close();
		}

	}
	void dayFourteenPartTwo() {
		std::system("cls"); // clear terminal pre-boot
		std::cout << "Running program DayFourteenPartTwo" << '\n';
		const bool isFullFile = true; // true   false
		const BigInt kXLen = isFullFile ? kXLenFull : kXLenTest;
		const BigInt kYLen = isFullFile ? kYLenFull : kYLenTest;

		std::string line;
		std::ifstream inputFile;
		if (isFullFile) {
			std::cout << "Using full file\n\n";
		}
		else {
			std::cout << "Using test file\n\n";
		}
		(isFullFile) ? inputFile.open("dayFourteenFull.txt") : inputFile.open("dayFourteenTest.txt");
		handleFile(inputFile, kXLen, kYLen);
	}
}
