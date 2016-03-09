#pragma once

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <glog/logging.h>

// A super duper simple config object based on boost's propery tree.
// Very little error checking is done. If a path is not prsesent on
// which we are doing get, this would raise an exception and abort
// the application. TODO(sgupta) Handle this in future


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

      template <typename T>
      T getValue(const std::string& path) const
      {
         T val = pt.get<T>(path);
         LOG(INFO) << "......[" << path <<"] " << val;
         return val;
      }

   private:
      boost::property_tree::ptree pt {};
};
