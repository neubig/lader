#ifndef CONFIG_BASE_H__
#define CONFIG_BASE_H__

#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <tr1/unordered_map>
#include <kyldr/util.h>

namespace kyldr {

#define DIE_HELP(msg) do {                      \
    std::std::ostringstream oss;                     \
    oss << msg;                                 \
    DieOnHelp(oss.str()); }                     \
  while (0);

class ConfigBase {

protected:

    // name -> value, description
    typedef unordered_map<std::string, pair<std::string,std::string> > ConfigMap;

    // argument functions
    int minArgs_, maxArgs_;   // min and max number of main arguments
    std::vector<std::string> mainArgs_; // the main non-optional argiments
    ConfigMap optArgs_;       // optional arguments 

    // details for printing the usage
    std::string usage_;            // usage details
    std::vector<std::string> argOrder_;

public:

    ConfigBase() : minArgs_(0), maxArgs_(255) { }

    void DieOnHelp(const std::string & str) const {
        // print arguments
        cerr << usage_ << endl;
        cerr << "Arguments: "<<endl;
        for(std::vector<std::string>::const_iterator it = argOrder_.begin(); 
                        it != argOrder_.end(); it++) {
            ConfigMap::const_iterator oit = optArgs_.find(*it);
            if(oit->second.second.length() != 0)
                cerr << " -"<<oit->first<<" \t"<<oit->second.second<<endl;
        }
        cerr << endl << str << endl;
        exit(1);
    }
    
    void PrintConf() const {
        // print arguments
        cout << "Main arguments:" << endl;
        for(int i = 0; i < (int)mainArgs_.size(); i++)
            cout << " "<<i<<": "<<mainArgs_[i]<<endl;
        cout << "Optional arguments:"<<endl;
        for(std::vector<std::string>::const_iterator it = argOrder_.begin(); it != argOrder_.end(); it++) {
            ConfigMap::const_iterator oit = optArgs_.find(*it);
            if(oit->second.second.length() != 0)
                cout << " -"<<oit->first<<" \t"<<oit->second.first<<endl;
        }
    }

    std::vector<std::string> loadConfig(int argc, char** argv) {
        for(int i = 1; i < argc; i++) {
            if(argv[i][0] == '-') {
                std::string name(argv[i]+1); 
                ConfigMap::iterator cit = optArgs_.find(name);
                if(cit == optArgs_.end())
                    DIE_HELP("Illegal argument "<<name);
                if(i == argc-1 || argv[i+1][0] == '-')
                    cit->second.first = "true";
                else
                    cit->second.first = argv[++i];
            }
            else
                mainArgs_.push_back(argv[i]);
        }

        // sanity checks
        if((int)mainArgs_.size() < minArgs_ || (int)mainArgs_.size() > maxArgs_) {
            DIE_HELP("Wrong number of arguments");
        }

        // method specific settings
        std::string sampMeth = getString("sampmeth");
        if(sampMeth == "sequence") {
            if(getInt("blocksize") > 1)
                THROW_ERROR("Blocksize > 1 ("<<getInt("blocksize")<<") for sequence sampling");
            if(getInt("threads") > 1)
                THROW_ERROR("Threads > 1 ("<<getInt("blocksize")<<") for sequence sampling");
        } else if(sampMeth == "parallel") {
            if(getInt("blocksize") > 1)
                THROW_ERROR("Blocksize > 1 ("<<getInt("blocksize")<<") for parallel sampling");
        } else if(sampMeth == "block") {
            // nothing to check for now
        } else if(sampMeth == "single") {
            // nothing to check for now
        } else {
            THROW_ERROR("Unknown sampling method "<<sampMeth);
        }
        

        PrintConf();
        return mainArgs_;
    }

    void addConfigEntry(const std::string & name, const std::string & val, const std::string & desc) {
        argOrder_.push_back(name);
        pair<std::string,pair<std::string,std::string> > entry(name,pair<std::string,std::string>(val,desc));
        optArgs_.insert(entry);
    }

    // getter functions
    std::string getString(const std::string & name) const {
        ConfigMap::const_iterator it = optArgs_.find(name);
        if(it == optArgs_.end())
            THROW_ERROR("Requesting bad argument "<<name<<" from configuration");
        return it->second.first;
    }
    int getInt(const std::string & name) const {
        std::string str = getString(name);
        int ret = atoi(str.c_str());
        if(ret == 0 && str != "0" && str != "00" && str != "000" && str != "0000")
            DIE_HELP("Value '"<<str<<"' for argument "<<name<<" was not an integer");
        return ret;
    }
    double getDouble(const std::string & name) const {
        std::string str = getString(name);
        double ret = atof(str.c_str());
        if(ret == 0 && str != "0" && str != "0.0")
            DIE_HELP("Value '"<<str<<"' for argument "<<name<<" was not float");
        return ret;
    }
    bool getBool(const std::string & name) const {
        std::string str = getString(name);
        if(str == "true") return true;
        else if(str == "false") return false;
        DIE_HELP("Value '"<<str<<"' for argument "<<name<<" was not boolean");
        return false;
    }

    // setter functions
    void setString(const std::string & name, const std::string & val) {
        ConfigMap::iterator it = optArgs_.find(name);
        if(it == optArgs_.end())
            THROW_ERROR("Setting bad argument "<<name<<" in configuration");
        it->second.first = val;
    }
    void setInt(const std::string & name, int val) {
        std::ostringstream oss; oss << val; setString(name,oss.str());
    }
    void setDouble(const std::string & name, double val) {
        std::ostringstream oss; oss << val; setString(name,oss.str());
    }
    void setBool(const std::string & name, bool val) {
        std::ostringstream oss; oss << val; setString(name,oss.str());
    }

    void setUsage(const std::string & str) { usage_ = str; }

    std::vector<std::string> getMainArgs() { return mainArgs_; }
	
};

}

#endif
