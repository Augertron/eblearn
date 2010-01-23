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

using namespace std;

namespace ebl {

  typedef map<string, string, less<string> > string_map_t;
  typedef map<string, vector<string>, less<string> > string_list_map_t;

  // utility functions
  string timestamp();
  
  class configuration {
  protected:
    string_map_t	smap;
    string name;
    string output_dir;
    
  public:

    //! empty constructor.
    configuration();

    //! load configuration found in filename.
    configuration(const char *filename);

    //! load configuration from already loaded map of variables, name and
    //! output directory.
    configuration(string_map_t &smap, string &name, string &output_dir);

    //! destructor.
    virtual ~configuration();

    //! load configuration from file fname.
    bool read(const char *fname);

    //! save configuration into file fname.
    bool write(const char *fname);

    // accessors

    const string &get_name();
    const string &get_output_dir();

    //! returns the string contained the variable with name varname.
    //! if varname does not exist, this throws an exception.
    const string &get_string(const char *varname);

    //! returns the string contained the variable with name varname.
    //! if varname does not exist, this throws an exception.
    const char *get_cstring(const char *varname);

    //! returns a double conversion of the string contained in the variable
    //! with name varname.
    //! if varname does not exist or the double conversion fails,
    //! this throws an exception.
    double get_double(const char *varname);

    //! returns a uint conversion of the string contained in the variable
    //! with name varname.
    //! if varname does not exist or the uint conversion fails,
    //! this throws an exception.
    uint get_uint(const char *varname);

    //! returns a bool conversion of the string contained in the variable
    //! with name varname.
    //! if varname does not exist or the uint conversion fails,
    //! this throws an exception.
    bool get_bool(const char *varname);

    //! set variable 'varname' to value 'value'
    void set(const char *varname, const char *value);
    
    //! print loaded variables
    virtual void pretty();
  };

  class meta_configuration : public configuration {
  private:
    string_map_t	tmpsmap;
    string_list_map_t	lmap;
    vector<size_t>	conf_indices;
    int			conf_combinations;
    vector<configuration> confs;
    
  public:
    meta_configuration();
    virtual ~meta_configuration();

    bool read(const char *fname);

    // accessors

    //! return all possible configurations
    vector<configuration>& configurations();

    //! print loaded variables
    virtual void pretty();
  };

} // end namespace ebl

#endif /* CONFIGURATION_H_ */
