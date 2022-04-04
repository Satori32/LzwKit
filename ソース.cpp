#include <iostream>
#include <vector>
#include <cstdint>
#include <random>
#include <limits>
#include <algorithm>

typedef std::vector<std::uint8_t> Data;
typedef std::uint16_t Word;
typedef std::vector<Word> DType;
typedef std::vector<Data> Datas;

Data MakeVector(std::size_t N, std::uint32_t Seed = 0) {
	std::mt19937 mt(Seed);
	//std::uniform_int_distribution<std::intmax_t> UI(0, std::numeric_limits<Data::value_type>::max());
	std::uniform_int_distribution<std::intmax_t> UI(0, 1);
	Data R;

	for (std::size_t i = 0; i < N; i++) {
		R.push_back(UI(mt));
	}

	return R;
}
Data MakeVector2(std::size_t N, std::uint32_t Seed = 0) {
	std::mt19937 mt(Seed);
	std::uniform_int_distribution<std::intmax_t> UI(0, std::numeric_limits<Data::value_type>::max());
	//std::uniform_int_distribution<std::intmax_t> UI(0, 1);
	Data R;

	for (std::size_t i = 0; i < N; i++) {
		R.push_back(UI(mt));
	}

	return R;
}
Data MakeVector3(std::size_t N, std::intmax_t Min,std::intmax_t Max, std::uint32_t Seed = 0) {
	std::mt19937 mt(Seed);
	std::uniform_int_distribution<std::intmax_t> UI(Min,Max);
	//std::uniform_int_distribution<std::intmax_t> UI(0, 1);
	Data R;

	for (std::size_t i = 0; i < N; i++) {
		R.push_back(UI(mt));
	}

	return R;
}
DType Lzw_Enc(const Data& D) {

	Datas Di;
	Data V;
	DType R;

	for (std::size_t i = 0; i <= std::numeric_limits<Data::value_type>::max(); i++) {
		V.push_back(i);
		Di.push_back(V);
		V.pop_back();
	}
	//R.push_back(0xfffe);//clear code.
	//R.push_back(0xffff);//stop code.

	for (std::size_t i = 0; i < D.size(); i++) {
		V.push_back(D[i]);

		auto it = std::find(Di.begin(), Di.end(), V);
		if (it != Di.end()) continue;
		
		auto A = V;
		A.pop_back();
		auto it2 = std::find(Di.begin(), Di.end(), A);
		
		if ( it == Di.end()) {
			auto L = std::distance(Di.begin(), it2);
			R.push_back(L);

			Di.push_back(V);
			V.clear();
			i--;
		}
	}
	if (V.size()) {
		auto It=std::find(Di.begin(), Di.end(), V);
		if (It == Di.end()) {
			R.push_back(Di.size());
		}
		else {
			R.push_back(std::distance(Di.begin(),It));
		}
	}

	return R;
}

Data Lzw_Dec(const DType& D, const Data& In) {

	Data V;
	Datas Di;
	Data R;

	for (std::size_t i = 0; i <= std::numeric_limits<Data::value_type>::max(); i++) {
		V.push_back(i);
		Di.push_back({ V });
		V.pop_back();
	}

	std::size_t i = 0;
	for (i = 0; i < D.size() - 1; i++) {
		V = Di[D[i]];
		Di.push_back(V);
		Di.back().push_back(Di[D[i + 1]].front());

		R.insert(R.end(), V.begin(), V.end());
	}
	//dirty. need the patch.
	V = Di[D[i]];
	Di.push_back(V);
	R.insert(R.end(), V.begin(), V.end());
	return R;
}

bool ShowE(const DType& In) {
	for (auto& o : In) {
		std::cout << (int)o << ',';
	}
	std::cout << std::endl;
	std::cout << std::endl;

	return true;
}
bool ShowD(const Data& In) {
	for (auto& o : In) {
		std::cout << (int)o << ',';
	}
	std::cout << std::endl;
	std::cout << std::endl;

	return true;
}
int main() {

	std::size_t L = 10240;

	auto D = MakeVector3(L,0,255);
	ShowD(D);
	DType RA = Lzw_Enc(D);
	ShowE(RA);

	Data RB=Lzw_Dec(RA,D);
	ShowD(RB);

	//D.pop_back();

	if (D == RB) {
		std::cout << "Good!" << std::endl;
	}
	else {
		std::cout << "Odd!" << std::endl;
	}
	return 0;
}