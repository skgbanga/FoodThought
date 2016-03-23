#pragma once

#include <regex>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <glog/logging.h>

// A super duper simple config object based on boost's propery tree.
// This class should handle recursion. See the test case

class ConfigObject
{
   public:
      explicit ConfigObject(const char* fileName)
      {
         boost::property_tree::ini_parser::read_ini(fileName, pt);
      }

      ConfigObject(const ConfigObject&) = delete;
      ConfigObject(ConfigObject&&) = delete;
      ConfigObject& operator=(const ConfigObject&) = delete;
      ConfigObject& operator=(ConfigObject&&) = delete;

      std::string getStringNoLog(const std::string& path, std::string def) const
      {
         // identify the section first by doing a regex on the path
         std::regex r1("(.*)\\.(.*)");
         std::smatch m1;
         auto result1 = std::regex_match(path, m1, r1);
         if (not result1)
            return "";

         std::string section = m1[1].str();

         // get the value stored in the variable now
         auto val1 = pt.get<std::string>(path, def);

         // see if we ned to make recursive call
         std::regex r2("(.*)\\$\\((.*)\\)(.*)"); // regex for things like abc$(def)ghi
         std::smatch m2;
         auto result2 = std::regex_match(val1, m2, r2);
         if (!result2) // if it doesn't match, then we are good
         {
            return val1;
         }
         else
         {
            auto token = m2[2].str();
            std::string tokenValue = "";

            // check if the token is an env variable
            if (auto* charPtr = std::getenv(token.c_str())) // check to see if env variable
               tokenValue =  std::string(charPtr);
            else
               tokenValue = getStringNoLog(section + "." + token, def);

            // otherwise
            auto resultingVal = m2[1].str() + tokenValue + m2[3].str();
            return resultingVal;
         }
      }

      std::string getString(const std::string& path, std::string def) const
      {
         auto val = getStringNoLog(path, def);
         LOG(INFO) << ".....[" << path <<"] " << val;
         return val;
      }

      int getInt(const std::string& path, int def) const
      {
         auto val = getStringNoLog(path, "");
         int ret = (val == "") ? def : std::atoi(val.c_str());
         LOG(INFO) << "......[" << path <<"] " << ret;
         return ret;
      }

      // returns default value if no conversion could not be performed
      double getDouble(const std::string& path, double def) const
      {
         auto val = getStringNoLog(path, "");
         auto ret = def;
         try
         {
            ret = std::stof(val.c_str());
         }
         catch (...)
         {
            // don't need to do anything.
            // ret is already set to default
         }
         LOG(INFO) << "......[" << path <<"] " << ret;
         return ret;
      }

   private:
      boost::property_tree::ptree pt {};
};
