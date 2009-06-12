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
    configuration() {}
    configuration(string_map_t &smap, string &name, string &output_dir);
    virtual ~configuration();

    bool read(const char *fname);
    bool write(const char *fname);

    // accessors

    const string &get_name();
    const string &get_output_dir();
  };

  class meta_configuration : public configuration {
  private:
    string_map_t	meta_smap;
    string_list_map_t	lmap;
    vector<size_t>	conf_indices;
    int			conf_combinations;
    vector<configuration> confs;
    
  public:
    meta_configuration();
    virtual ~meta_configuration();

    bool read(const char *fname);

    // accessors

    // return all possible configurations
    vector<configuration>& configurations();
  };

} // end namespace ebl

#endif /* CONFIGURATION_H_ */
