#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "heatmap.h"

namespace po = boost::program_options;

AllStatistics all("heatmap_file.txt");
Fingers fingers;
Names names;

typedef std::vector<std::string> TableLine;
typedef std::vector<TableLine> Table;

//-----------------------------------------------------------------------------
void tablePrint(const Table& table, bool isUseTab) {
	std::vector<int> sizes(table[0].size(), 0);
	for (auto& i : table) {
		if (i.size() != sizes.size())
			throw std::exception();
		for (int j = 0; j < i.size(); ++j)
			sizes[j] = std::max<size_t>(sizes[j], i[j].size());
	}

	for (int i = 0; i < table.size(); ++i) {
		if (isUseTab) {
			for (int j = 0; j < table[i].size(); ++j)
				std::cout << table[i][j] << (i != table[i].size()-1) ? "\t" : "";
		} else {
			for (int j = 0; j < table[i].size(); ++j)
				std::cout << std::setw(sizes[j]+1) << table[i][j];
		}
		std::cout << std::endl;
	}
}

//-----------------------------------------------------------------------------
std::string getPercent(double a, double all) {
	std::stringstream sout;
	sout << std::setprecision(2) << std::fixed << 100.0*a/all;
	return sout.str();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int sumLayerTaps(int layer);
int sumAllTaps(void);
int sumFingerTapsLayer(int finger, int layer);
int sumFingerTaps(int finger);
int sumRowTapsLayer(int row, int layer);
int sumRowTaps(int row);

//-----------------------------------------------------------------------------
int sumTapTaps(Tap tap) {
	int32_t sum = 0;
	for (const auto& i : all.twotap[tap]) {
		sum += all.twotap.getTapCount(tap, i);
		if (tap != i)
			sum += all.twotap.getTapCount(i, tap);
	}
	return sum;	
}

//-----------------------------------------------------------------------------
int sumLayerTaps(int layer) {
	if (layer == -1)
		return sumAllTaps();

	int32_t sum = 0;
	for (int j = 0; j < all.onetap.rowsCount(); ++j) {
		for (int k = 0; k < all.onetap.colsCount(); ++k) {
			sum += all.onetap.getTapCount({k, j, layer});
		}
	}
	return sum;	
}

//-----------------------------------------------------------------------------
int sumAllTaps(void) {
	int32_t sum = 0;
	for (int i = 0; i < all.onetap.layersCount(); ++i)
		sum += sumLayerTaps(i);
	return sum;
}

//-----------------------------------------------------------------------------
int sumFingerTapsLayer(int finger, int layer) {
	if (layer == -1)
		return sumFingerTaps(finger);

	int32_t sum = 0;
	for (int j = 0; j < all.onetap.rowsCount(); ++j) {
		for (int k = 0; k < all.onetap.colsCount(); ++k) {
			Tap tap{k, j, layer};
			if (finger == 10) {
				if (fingers.getFinger(tap) == 0 && fingers.getRow(tap) == 0)
					sum += all.onetap.getTapCount(tap);
			} else {
				if (fingers.getFinger(tap) == finger && fingers.getRow(tap) != 0)
					sum += all.onetap.getTapCount(tap);
			}
		}
	}
	return sum;
}

//-----------------------------------------------------------------------------
int sumFingerTaps(int finger) {
	int32_t sum = 0;
	for (int i = 0; i < all.onetap.layersCount(); ++i)
		sum += sumFingerTapsLayer(finger, i);
	return sum;
}

//-----------------------------------------------------------------------------
int sumRowTapsLayer(int row, int layer) {
	if (layer == -1) 
		return sumRowTaps(row);

	int32_t sum = 0;
	for (int j = 0; j < all.onetap.rowsCount(); ++j) {
		for (int k = 0; k < all.onetap.colsCount(); ++k) {
			Tap tap{k, j, layer};
			if (row == 0) {
				if (fingers.getRow(tap) == 0 && fingers.getFinger(tap) == 0)
					sum += all.onetap.getTapCount(tap);
			} else {
				if (fingers.getRow(tap) == row)
					sum += all.onetap.getTapCount(tap);
			}
		}
	}
	return sum;
}

//-----------------------------------------------------------------------------
int sumRowTaps(int row) {
	int32_t sum = 0;
	for (int i = 0; i < all.onetap.layersCount(); ++i)
		sum += sumRowTapsLayer(row, i);
	return sum;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void sum_all(const po::variables_map& vm) {
	bool isUseTabs = vm.count("use-tabs");
	if (!vm.count("separate-by-layers")) {
		std::cout << "Count of all taps: " << sumAllTaps() << std::endl;
	} else {
		auto allSum = sumAllTaps();
		Table table;
		TableLine head;
		head.push_back("Layer");
		head.push_back("Taps");
		head.push_back("Percent");
		table.emplace_back(head);
		for (int i = 0; i < all.onetap.layersCount(); ++i) {
			auto layerSum = sumLayerTaps(i);

			TableLine line;
			line.push_back(std::to_string(i));
			line.push_back(std::to_string(layerSum));
			line.push_back(getPercent(layerSum, allSum) + "%");
			
			table.emplace_back(line);
		}
		tablePrint(table, isUseTabs);
	}
}

//-----------------------------------------------------------------------------
void sum_fingers(const po::variables_map& vm) {
	bool isUseTabs = vm.count("use-tabs");
	auto print_for_layer = [&] (int layer) {
		auto allSum = sumLayerTaps(layer);
		Table table;
		TableLine head;
		head.push_back("Hand");
		head.push_back("Finger");
		head.push_back("Taps");
		head.push_back("Percent");
		table.emplace_back(head);
		for (int i = 0; i <= 10; ++i) {
			auto fingerSum = sumFingerTapsLayer(i, layer);

			TableLine line;
			line.push_back(fingers.getHandName(i));
			line.push_back(fingers.getFingerName(i));
			line.push_back(std::to_string(fingerSum));
			line.push_back(getPercent(fingerSum, allSum) + "%");
			
			table.emplace_back(line);
		}
		tablePrint(table, isUseTabs);
	};

	if (!vm.count("separate-by-layers")) {
		print_for_layer(-1);
	} else {
		for (int i = 0; i < all.onetap.layersCount(); ++i) {
			std::cout << "For layer: " << i << std::endl;
			print_for_layer(i);
			if (i+1 != all.onetap.layersCount())
				std::cout << std::endl; 
		}
	}
}

//-----------------------------------------------------------------------------
void sum_rows(const po::variables_map& vm) {
	bool isUseTabs = vm.count("use-tabs");
	auto print_for_layer = [&] (int layer) {
		auto allSum = sumLayerTaps(layer);
		Table table;
		TableLine head;
		head.push_back("Row");
		head.push_back("Taps");
		head.push_back("Percent");
		table.emplace_back(head);
		for (int i = 5; i >= 0; --i) {
			auto rowSum = sumRowTapsLayer(i, layer);

			TableLine line;
			line.push_back(fingers.getRowName(i));
			line.push_back(std::to_string(rowSum));
			line.push_back(getPercent(rowSum, allSum) + "%");
			
			table.emplace_back(line);
		}
		tablePrint(table, isUseTabs);
	};

	if (!vm.count("separate-by-layers")) {
		print_for_layer(-1);
	} else {
		for (int i = 0; i < all.onetap.layersCount(); ++i) {
			std::cout << "For layer: " << i << std::endl;
			print_for_layer(i);
			if (i+1 != all.onetap.layersCount())
				std::cout << std::endl; 
		}
	}
}

//-----------------------------------------------------------------------------
void sum(const po::variables_map& vm) {
	std::string regime, fingerfile;
	if (vm.count("regime"))
		regime = vm["regime"].as<std::string>();
	if(vm.count("fingerfile")) {
		fingerfile = vm["fingerfile"].as<std::string>();

		// Открыть файл и проверить его
		try {
			std::ifstream fin(fingerfile);
			if (fin) {
				fingers.load(fin);
				fin.close();
			} else {
				std::cout << "Fingers file didn't exists!" << std::endl;
				return;
			}
		} catch (std::exception& ex) {
			std::cout << "Fingers file is wrong! Exception: " << ex.what() << std::endl;
			return;
		}
	} else if (regime == "fingers" || regime == "rows") {
		std::cout << "You not enter file with fingers specification." << std::endl;
	}
	
	if (regime == "all") {
		sum_all(vm);
	} else if (regime == "fingers") {
		sum_fingers(vm);	
	} else if (regime == "rows") {
		sum_rows(vm);
	} else {
		std::cout << "Unknown regime `" << regime << "` when using sum type." << std::endl;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct tap_elem
{
	std::string name;
	int taps;
};

//-----------------------------------------------------------------------------
void sum_equal_keys(std::vector<tap_elem>& taps) {
	for (auto i = taps.begin(); i != taps.end(); ++i) {
		for (auto j = std::next(i); j != taps.end(); ++j) {
			start:
			if (i->name == j->name) {
				i->taps += j->taps;
				j = taps.erase(j);
				if (j == taps.end()) break;
				goto start;
			}
		}
	}
}

//-----------------------------------------------------------------------------
void onetap(const po::variables_map& vm) {
	bool isUseNames = false;
	if (vm.count("namefile")) {
		std::string name = vm["namefile"].as<std::string>(); 
		std::ifstream fin(name);
		try {
			if (fin) {
				names.load(fin);
				fin.close();
				isUseNames = true;
			} else {
				std::cout << "Names file didn't exists!" << std::endl;
					return;
			}
		} catch (std::exception& ex) {
			std::cout << "Names file is wrong! Exception: " << ex.what() << std::endl;
			return;
		}
	}

	bool isSumEqualKeys = vm.count("sum-equal-keys");
	bool isUseTabs = vm.count("use-tabs");
	bool isShowZeros = vm.count("show-zeros");
	bool isSeparateByLayers = vm.count("separate-by-layers");
	bool isShowPosWithName = vm.count("show-pos-with-name");
	int substr = -1; if (vm.count("substr")) substr = vm["substr"].as<int>();
	std::string sort = "frequent"; if (vm.count("sort")) sort = vm["sort"].as<std::string>();
	auto make_for_layer = [&] (std::vector<tap_elem>& elems, int layer) {
		for (int j = 0; j < all.onetap.rowsCount(); ++j) {
			for (int k = 0; k < all.onetap.colsCount(); ++k) {
				Tap tap{k, j, layer};
				int taps = all.onetap.getTapCount(tap);
				if (!isShowZeros && taps != 0) {
					std::string name;
					if (isShowPosWithName || !isUseNames) 
						name = std::to_string(tap.layer) + "," + std::to_string(tap.row) + "," + std::to_string(tap.col);
					if (isShowPosWithName && isUseNames)
						 name = name +  "," + names.getName(tap);
					if (!isShowPosWithName && isUseNames)
						name = names.getName(tap);
					elems.push_back({name, taps});
				}
			}
		}
		if (isSumEqualKeys) sum_equal_keys(elems);
	};
	auto make_for_layers = [&] (std::vector<tap_elem>& elems) {
		for (int i = 0; i < all.onetap.layersCount(); ++i)
			make_for_layer(elems, i);
		if (isSumEqualKeys) sum_equal_keys(elems);
	};

	auto sort_elems = [&] (std::vector<tap_elem>& elems) {
		if (sort == "frequent") {
			std::sort(elems.begin(), elems.end(), [] (auto& a, auto& b) -> bool {
				return a.taps > b.taps;
			});
		} else if (sort == "rare") {
			std::sort(elems.begin(), elems.end(), [] (auto& a, auto& b) -> bool {
				return a.taps < b.taps;
			});
		} else {
			// Никакой сортировки!
		}
	};

	auto print_elems = [&] (std::vector<tap_elem>& elems, int allSum) {
		if (substr == -1 || substr > elems.size()) substr = elems.size();
		Table table;
		TableLine head;
		head.push_back("Name");
		head.push_back("Taps");
		head.push_back("Percent");
		table.emplace_back(head);
		for (int i = 0; i < substr; ++i) {
			TableLine line;
			line.push_back(elems[i].name);
			line.push_back(std::to_string(elems[i].taps));
			line.push_back(getPercent(elems[i].taps, allSum) + "%");

			table.emplace_back(line);
		}
		tablePrint(table, isUseTabs);
	};

	if (isSeparateByLayers) {
		for (int i = 0; i < all.onetap.layersCount(); ++i) {
			std::cout << "Layer: " << i << std::endl;
			std::vector<tap_elem> elems;
			make_for_layer(elems, i);
			sort_elems(elems);
			print_elems(elems, sumLayerTaps(i));
			std::cout << std::endl;
		}
	} else {
		std::vector<tap_elem> elems;
		make_for_layers(elems);
		sort_elems(elems);
		print_elems(elems, sumAllTaps());
	}

}

//-----------------------------------------------------------------------------
void twotap(const po::variables_map& vm) {
	bool isUseNames = false;
	if (vm.count("namefile")) {
		std::string name = vm["namefile"].as<std::string>(); 
		std::ifstream fin(name);
		try {
			if (fin) {
				names.load(fin);
				fin.close();
				isUseNames = true;
			} else {
				std::cout << "Names file didn't exists!" << std::endl;
					return;
			}
		} catch (std::exception& ex) {
			std::cout << "Names file is wrong! Exception: " << ex.what() << std::endl;
			return;
		}
	}

	bool isSumEqualKeys = vm.count("sum-equal-keys");
	bool isUseTabs = vm.count("use-tabs");
	bool isShowZeros = vm.count("show-zeros");
	bool isSeparateByLayers = vm.count("separate-by-layers");
	bool isSeparateByKeys = vm.count("separate-by-keys");
	bool isShowPosWithName = vm.count("show-pos-with-name");
	bool isPrintFirstName = !vm.count("two-names");
	int substr = -1; if (vm.count("substr")) substr = vm["substr"].as<int>();
	std::string sort = "frequent"; if (vm.count("sort")) sort = vm["sort"].as<std::string>();
	auto get_tap_name = [&] (Tap tap) {
		std::string name;
		if (isShowPosWithName || !isUseNames) 
			name = std::to_string(tap.layer) + "," + std::to_string(tap.row) + "," + std::to_string(tap.col);
		if (isShowPosWithName && isUseNames)
			name = name +  "," + names.getName(tap);
		if (!isShowPosWithName && isUseNames)
			name = names.getName(tap);
		return name;
	};
	auto make_for_tap = [&] (std::vector<tap_elem>& elems, Tap tap1, std::string tap1_name) {
		auto add_taps = [&] (Tap tap1, Tap tap2) {
			int taps = all.twotap.getTapCount(tap1, tap2);
			if (!isShowZeros && taps != 0) {
				std::string tap2_name = get_tap_name(tap2);

				std::string twotap_name;
				if (tap1_name.size() == 0) {
					twotap_name = tap2_name;
				} else if (tap1_name.size() == 1 && tap2_name.size() == 1) {
					twotap_name = tap1_name + tap2_name;
				} else {
					twotap_name = tap1_name + " + " + tap2_name;
				}

				elems.push_back({twotap_name, taps});
			}
		};
		for (const auto& i : all.twotap[tap1]) {
			add_taps(tap1, i);
		}
		if (isSeparateByKeys) 
			for (const auto& i : all.twotap) {
				tap1_name = get_tap_name(i);
				add_taps(i, tap1);
			}
		if (isSumEqualKeys) sum_equal_keys(elems);
	};
	auto make_for_layer = [&] (std::vector<tap_elem>& elems, int layer) {
		for (int j1 = 0; j1 < all.twotap.rowsCount(); ++j1) {
			for (int k1 = 0; k1 < all.twotap.colsCount(); ++k1) {
				Tap tap1{k1, j1, layer};
				make_for_tap(elems, tap1, get_tap_name(tap1));
			}
		}
		if (isSumEqualKeys) sum_equal_keys(elems);
	};
	auto make_for_layers = [&] (std::vector<tap_elem>& elems) {
		for (int i = 0; i < all.twotap.layersCount(); ++i)
			make_for_layer(elems, i);
		if (isSumEqualKeys) sum_equal_keys(elems);
	};

	auto sort_elems = [&] (std::vector<tap_elem>& elems) {
		if (sort == "frequent") {
			std::sort(elems.begin(), elems.end(), [] (auto& a, auto& b) -> bool {
				return a.taps > b.taps;
			});
		} else if (sort == "rare") {
			std::sort(elems.begin(), elems.end(), [] (auto& a, auto& b) -> bool {
				return a.taps < b.taps;
			});
		} else {
			// Никакой сортировки!
		}
	};

	auto print_elems = [&] (std::vector<tap_elem>& elems, int allSum) {
		int loc_substr = substr;
		if (loc_substr == -1 || loc_substr > elems.size()) loc_substr = elems.size();
		Table table;
		TableLine head;
		head.push_back("Name");
		head.push_back("Taps");
		head.push_back("Percent");
		table.emplace_back(head);
		for (int i = 0; i < loc_substr; ++i) {
			TableLine line;
			line.push_back(elems[i].name);
			line.push_back(std::to_string(elems[i].taps));
			line.push_back(getPercent(elems[i].taps, allSum) + "%");

			table.emplace_back(line);
		}
		tablePrint(table, isUseTabs);
	};

	if (isSeparateByKeys) {
		for (int i = 0; i < all.twotap.layersCount(); ++i) {
			std::cout << "Layer: " << i << std::endl;
			for (int j1 = 0; j1 < all.twotap.rowsCount(); ++j1) {
				for (int k1 = 0; k1 < all.twotap.colsCount(); ++k1) {
					Tap tap{k1, j1, i};
					std::vector<tap_elem> elems;
					if (isPrintFirstName)
						make_for_tap(elems, tap, get_tap_name(tap));
					else
						make_for_tap(elems, tap, "");
					sort_elems(elems);
					if (isShowZeros && elems.size() == 0) {
						std::cout << "Key: " << get_tap_name(tap) << ", zero occurences." << std::endl;
					} else if (elems.size() != 0) {
						std::cout << "Key: " << get_tap_name(tap) << std::endl;
						print_elems(elems, sumTapTaps(tap));
						std::cout << std::endl;
					}
				}
			}
			std::cout << std::endl;
		}
	} else if (isSeparateByLayers) {
		for (int i = 0; i < all.twotap.layersCount(); ++i) {
			std::cout << "Layer: " << i << std::endl;
			std::vector<tap_elem> elems;
			make_for_layer(elems, i);
			sort_elems(elems);
			print_elems(elems, sumLayerTaps(i));
			std::cout << std::endl;
		}
	} else {
		std::vector<tap_elem> elems;
		make_for_layers(elems);
		sort_elems(elems);
		print_elems(elems, sumAllTaps());
	}

}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void daily(const po::variables_map& vm) {
	bool isUseTabs = vm.count("use-tabs");
	int days = 31;
	if (vm.count("period")) {
		auto period = vm["period"].as<std::string>();
		if (period == "today") days = 0;
		if (period == "yestrday") days = 1;
		if (period == "week") days = 7;
		if (period == "month") days = 31;
		if (period == "year") days = 366;
		if (period == "all") days = -1;
	}

	auto print_date = [&] (int day) -> std::string {
		std::stringstream sout;
		sout << all.daytap.getDay(day) << "."
		     << all.daytap.getMonth(day) << "."
		     << all.daytap.getYear(day);
		return sout.str();
	};

	int today = all.daytap.getToday();
	Table table;
	TableLine head;
	head.push_back("Date");
	head.push_back("Taps");
	table.emplace_back(head);
	for (auto& i : all.daytap.getStatistics()) {
		if (today-i.first <= days || days == -1) {
			TableLine line;
			line.push_back(print_date(i.first));
			line.push_back(std::to_string(i.second));
			table.emplace_back(line);
		}
	}
	tablePrint(table, isUseTabs);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


int main(int argc, char* argv[]) {
	po::options_description desc("General options");
	desc.add_options()
		("help,h", "Show help")
		("statfile,s", po::value<std::string>(), "File with statistics. For default it is `heatmap_file.txt`.")
		("type,t", po::value<std::string>(), "Select statistics type: `sum`, `onetap`, `twotap`, "
			"`daily`."
			"\n  \t`sum` - sum keys count in different ways."
			"\n  \t`onetap` - statistics of one tap."
			"\n  \t`twotap` - count of two serial taps."
			"\n  \t`daily` - statistics by each day."
			);

	po::options_description sum_desc("`sum` options");
	sum_desc.add_options()
		("regime,r", po::value<std::string>(), "Select sum regime: `all`, `fingers`, `rows`.\n  \t`all` - write count of all taps.\n  \t`fingers` - write count of taps by fingers.\n  \t`rows` - write count of taps by rows.")
		("fingerfile,f", po::value<std::string>(), "File with fingers and rows specification")
		("separate-by-layers,S", "Separate any statistics by layers.");

	po::options_description onetap_desc("`onetap` options");
	onetap_desc.add_options()
		("show-zeros,z", "Show keys with zero taps.")
		("use-tabs,b", "Using tabs instead of spaces in tables. It may be useful ta make csv from statistics.")
		("separate-by-layers,S", "Separate any statistics by layers.")
		("sort,r", po::value<std::string>(), "Can be: `frequent`, `no`, `rare`. Sorted statistics by taps count. If frequent is selected, then most frequent keys will be on the top.")
		("namefile,f", po::value<std::string>(), "File with key names. If this file is specified, then position of key isn't displayed. To display it, use next option.")
		("show-pos-with-name,p", "When file with keys names is specified, prints position in format: layer,row,col,\"Name\". Example: 0,1,1,\"A\".")
		("sum-equal-keys,q", "Keys with equal names will be summed up.")
		("substr,u", po::value<int>(), "Integer value N. Shows only N first results.");

	po::options_description twotap_desc("`twotap` options");
	twotap_desc.add_options()
		("show-zeros,z", "Show keys with zero taps.")
		("use-tabs,b", "Using tabs instead of spaces in tables. It may be useful ta make csv from statistics.")
		("separate-by-layers,S", "Separate any statistics by layers.")
		("separate-by-keys,K", "Separate this statistics by keys.")
		("two-names,T", "With `separate-by-keys` option didn't print first key in each occurence:\n\tKey e:\na 1.81%\nb 1.5%\n...")
		("sort,r", po::value<std::string>(), "Can be: `frequent`, `no`, `rare`. Sorted statistics by taps count. If frequent is selected, then most frequent keys will be on the top.")
		("namefile,f", po::value<std::string>(), "File with key names. If this file is specified, then position of key isn't displayed. To display it, use next option.")
		("show-pos-with-name,p", "When file with keys names is specified, prints position in format: layer,row,col,\"Name\". Example: 0,1,1,\"A\".")
		("sum-equal-keys,q", "Keys with equal names will be summed up.")
		("substr,u", po::value<int>(), "Integer value N. Shows only N first results.");

	po::options_description daily_desc("`daily` options");
	daily_desc.add_options()
		("period,p", po::value<std::string>(), "Can be: `today`, `yesterday`, `week`, `month`, `year`, `all`.")
		("sort-by-taps,S", "For default sorted by day. With this option stats will be sorted by taps count.");

	po::variables_map vm;
	try {
		po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
		po::store(parsed, vm);
		po::notify(vm);

		if (vm.count("statfile"))
			all.file = vm["statfile"].as<std::string>();

		all.load();

		if (vm.count("type")) {
			auto type = vm["type"].as<std::string>();
			if (type == "sum") {
				desc.add(sum_desc);
				po::store(po::parse_command_line(argc, argv, desc), vm);
				sum(vm);
			} else if (type == "onetap") {
				desc.add(onetap_desc);
				po::store(po::parse_command_line(argc, argv, desc), vm);
				onetap(vm);
			} else if (type == "twotap") {
				desc.add(twotap_desc);
				po::store(po::parse_command_line(argc, argv, desc), vm);
				twotap(vm);
			} else if (type == "daily") {
				desc.add(daily_desc);
				po::store(po::parse_command_line(argc, argv, desc), vm);
				daily(vm);
			} else {
				std::cout << "Unknown type `" << type << "`." << std::endl;
			}
		} else if (vm.count("help")) {
			desc.add(sum_desc).add(onetap_desc).add(daily_desc);
			std::cout << desc << std::endl;
		} else {
			std::cout << "You don't specify regime! Try `--help` for more information." << std::endl; 
		}
	} catch(std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}

	//system("pause");
	return 0;
}