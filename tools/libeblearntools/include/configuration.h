/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Redistribution under a license not approved by the Open Source 
 *       Initiative (http://www.opensource.org) must display the 
 *       following acknowledgement in all advertising material:
 *        This product includes software developed at the Courant
 *        Institute of Mathematical Sciences (http://cims.nyu.edu).
 *     * The names of the authors may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ThE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <map>
#include <vector>
#include "defines.h"
#include "utils.h"
#include <list>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////

  typedef map<string, string, less<string> > string_map_t;
  typedef map<string, vector<string>, less<string> > string_list_map_t;
  
  ////////////////////////////////////////////////////////////////
  //! a class containing the original text of the configuration file in a
  //! list form. once variables have been updated, they can be inserted back
  //! to this list to recreate the original file with the new values.
  class textlist : public list< pair<string,string> > {
  public:
    //! constructor.
    textlist();

    //! deep copy constructor.
    textlist(const textlist &txt);

    //! destructor.
    virtual ~textlist();

    //! replace original line with variable assignment to value, everywhere that
    //! is indicated by a second element equal to varname.
    //! if variable is not found, push it at the end of the list.
    void update(const string &varname, const string &value);

    //! write entire text to out (using new variables if updated).
    void print(ostream &out);
  };

  ////////////////////////////////////////////////////////////////
  // utility functions

  string timestamp();
  
  ////////////////////////////////////////////////////////////////
  //! configuration class. handle files containing variable definitions.
  class EXPORT configuration {
  protected:
    string_map_t	smap; 	//!< map between variables and values
    string_map_t	tmp_smap; //!< map between variables and values
    string 		name; 	//!< name of configuration
    string 		output_dir;	//!< output directory
    textlist 		otxt; 	//!< original text
    
  public:
    //! empty constructor.
    configuration();

    //! copy constructor.
    configuration(const configuration &other);

    //! load configuration found in filename.
    configuration(const char *filename, bool replquotes = false);

    //! load configuration found in filename.
    configuration(const string &filename, bool replquotes = false);

    //! load configuration from already loaded map of variables, name and
    //! output directory.
    configuration(string_map_t &smap, textlist &txt, string &name, 
		  string &output_dir);

    //! destructor.
    virtual ~configuration();

    //! load configuration from file fname.
    bool read(const char *fname, bool bresolve = true, bool replquotes = true,
	      bool silent = false);

    //! save configuration into file fname.
    bool write(const char *fname);

    //! resolve variables names in variables
    //! @param replquotes Remove quotes or not from variable strings.
    void resolve(bool replquotes = false);

    // accessors

    //! return the name of the configuration
    const string &get_name();

    //! return output directory.
    const string &get_output_dir();

    //! Generic template get, return the value associated with varname,
    //! if varname exists, otherwise throws an execption.
    template <typename T>
      void get(T &v, const char *varname);

    //! Generic template get, return the value associated with varname,
    //! if varname exists, otherwise throws an execption.
    template <typename T>
      void get(T &v, const std::string &varname);

    //! Get variable with name varname as an intg into v.
    void get(intg &v, const char *varname);
    //! Get variable with name varname as an uint into v.
    void get(uint &v, const char *varname);
    //! Get variable with name varname as a double into v.
    void get(double &v, const char *varname);
    //! Get variable with name varname as a string into v.
    void get(string &v, const char *varname);

    //! returns the string contained the variable with name varname.
    //! if varname does not exist, this throws an exception.
    const string &get_string(const char *varname);

    //! returns the string contained the variable with name varname.
    //! if varname does not exist, this throws an exception.
    const string &get_string(const string &varname);

    //! returns the string contained the variable with name varname.
    //! if varname does not exist, this throws an exception.
    const char *get_cstring(const char *varname);

    //! returns a double conversion of the string contained in the variable
    //! with name varname.
    //! if varname does not exist or the double conversion fails,
    //! this throws an exception.
    double get_double(const char *varname);

    //! returns a float conversion of the string contained in the variable
    //! with name varname.
    //! if varname does not exist or the float conversion fails,
    //! this throws an exception.
    float get_float(const char *varname);

    //! returns a uint conversion of the string contained in the variable
    //! with name varname.
    //! if varname does not exist or the uint conversion fails,
    //! this throws an exception.
    uint get_uint(const char *varname);

    //! returns an int conversion of the string contained in the variable
    //! with name varname.
    //! if varname does not exist or the int conversion fails,
    //! this throws an exception.
    int get_int(const char *varname);

    //! Returns an intg conversion of the string contained in the variable
    //! with name varname.
    //! if varname does not exist or the int conversion fails,
    //! this throws an exception.
    intg get_intg(const char *varname);

    //! returns a bool conversion of the string contained in the variable
    //! with name varname.
    //! if varname does not exist or the uint conversion fails,
    //! this throws an exception.
    bool get_bool(const char *varname);

    //! Returns true if variable exists and its value is true, false otherwise.
    bool exists_bool(const char *varname);

    //! Returns true if variable exists and its value is true, false otherwise.
    bool exists_true(const char *varname);

    //! Returns true if variable exists and its value is false, true otherwise.
    bool exists_false(const char *varname);

    //! Checks that variable varname exists in the map,
    //! if not throw an exception.
    void exists_throw(const char *varname);

    //! set variable 'varname' to value 'value'
    void set(const char *varname, const char *value);

    //! get variable 'varname'.
    //! \param silent If false, warn when using env variable.
    const char* get_cstr(const char *varname, bool silent = false);

    //! returns true if the variable exists, false otherwise.
    bool exists(const char *varname);
    
    //! returns true if the variable exists, false otherwise.
    bool exists(const std::string &varname);
    
    //! print loaded variables
    virtual void pretty();
  };

  ////////////////////////////////////////////////////////////////
  //! meta configuration. derive from configuration, to handle meta variables.
  class EXPORT meta_configuration : public configuration {
  private:
    string_list_map_t	lmap;
    vector<size_t>	conf_indices;
    int			conf_combinations;
    vector<configuration> confs;
    
  public:
    meta_configuration();
    virtual ~meta_configuration();

    bool read(const char *fname, bool bresolve = true,
	      const string *tstamp = NULL, 
	      bool replace_quotes = false);

    // accessors

    //! return all possible configurations
    vector<configuration>& configurations();

    //! print loaded variables
    virtual void pretty();
  };

} // end namespace ebl

#include "configuration.hpp"

#endif /* CONFIGURATION_H_ */
