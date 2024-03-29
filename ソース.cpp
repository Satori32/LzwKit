#include <iostream>
#include <vector>
#include <cstdint>
#include <random>
#include <limits>
#include <algorithm>
#include <fstream>
#include <iterator>

typedef std::vector<std::uint8_t> Data;
typedef std::uint16_t Word;
typedef std::vector<Word> DType;
typedef std::vector<Data> Datas;



Data LoadFromFile(const std::string& Name) {
	std::ifstream ifs(Name, std::ios::binary);

	if (!ifs.is_open()) { return {}; }

	std::istreambuf_iterator<std::ifstream::char_type> it(ifs);
	std::istreambuf_iterator<std::ifstream::char_type> End;

	return { it,End };
}
	

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

DType Lzw_EncII(Data D, std::size_t DC = 256) {
	Datas Dc;
	Data  X;
	Data Y;
	DType R;


	for (std::size_t i = 0; i < DC; i++) {
		Data Y;
		Y.push_back(i);
		Dc.push_back(Y);
	}

	for (std::size_t i = 0; i < D.size(); i++) {
		Y = X;
		X.push_back(D[i]);
		auto it = std::find(Dc.begin(), Dc.end(), X);
		if (it == Dc.end()) {
			auto it2 = std::find(Dc.begin(), Dc.end(), Y);
			auto L = std::distance(Dc.begin(), it2);
			R.push_back(L);
			Dc.push_back(X);
			X.clear();
			Y.clear();
			i--;
		}
	}
	if (X.size()) {
		auto it = std::find(Dc.begin(), Dc.end(), X);
		auto L = std::distance(Dc.begin(), it);
		R.push_back(L);
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
			if (i != 0) { i--; }
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

Data Lzw_Dec(const DType& D){// , const Data& In) {

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
	std::cout.flush();
	for (auto& o : In) {
		std::cout << (int)o << ',';
	}
	std::cout << std::endl;
	std::cout << std::endl;

	return true;
}
bool ShowD(const Data& In) {
	std::cout.flush();
	for (auto& o : In) {
		std::cout << (int)o << ',';
	}
	std::cout << std::endl;
	std::cout << std::endl;

	return true;
}
/**/
Data WordToByte(const DType& In) {
	Data R;

	for (auto& o : In) {
		R.push_back(o / 256);
		R.push_back(o % 256);

	}

	return R;
}
DType ByteToWord(const Data& In) {
	DType R;
	for (std::size_t i =1; i < In.size(); i += 2) {
		Word W = In[i] + In[i-1] * 256;
		R.push_back(W);
	}

	return R;
}
/**/
/** /
Data WordToByte(const DType& In) {
	Data R;

	for (auto& o : In) {
		R.push_back(o % 256);
		R.push_back(o / 256);
	}

	return R;
}
DType ByteToWord(const Data& In) {
	DType R;
	for (std::size_t i = 0; i < In.size(); i += 2) {
		Word W = In[i] + In[i + 1] * 256;
		R.push_back(W);
	}

	return R;
}
/**/

/** /
int main() {
	for (std::size_t i = 0; i < 65536; i++) {
		auto A = i / 256;
		auto B = i % 256;

		auto C = B + A * 256;

		if (C == i) {
			std::cout.flush();
			std::cout << i << ": valid."<<std::endl;
		}
		else {
			break;
		}
		
	}

	return 0;

}
/**/
/**/
int main() {

	std::size_t L = 1024;

	std::size_t C = 1;
	bool IsFile =false;
	Data D;
	if (IsFile)
	{
		D = LoadFromFile("A.bmp");
	}
	else {
		D = MakeVector3(L, 0, 255, 0);
	}
	ShowD(D);

	Data T = D;
	DType RA;
	for (std::size_t i = 0; i < C; i++) {
		RA = Lzw_EncII(D);
		ShowE(RA);
		std::cout << "Comp:"<<RA.size()<<':' << RA.size() / (double)T.size() << std::endl;
		D = WordToByte(RA);
	}

	std::cout << std::endl << "----EncEnd---" << std::endl;
	Data RB;
	for (std::size_t i = 0; i < C; i++) {
		RB = Lzw_Dec(RA);
		RA = ByteToWord(RB);
		if (i + 1 != C) {
			ShowE(RA);
		}
		else {
			ShowD(RB);
		}
	}


	if (T == RB) {
		std::cout << "Good!" << std::endl;
	}
	else {
		std::cout << "Odd!" << std::endl;
	}
	return 0;
}
/** /
int main() {

	std::size_t L = 10240;

	auto D = MakeVector3(L,0,255,0);
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
/**/