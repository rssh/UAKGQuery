#ifndef __Utils_SzArray_h
#define __Utils_SzArray_h

/*
 * SzArray.h
 * part of implementation of CORBA Query service for Oracle.
 * (c) Grad-Soft Ltd <http://www.gradsoft.kiev.ua>, 2000-2003
 * $Id: SzArray.h,v 1.15 2003-03-11 19:25:22 rssh Exp $
 */

#ifndef __importCORBA_h
#include <importCORBA.h>
#endif

#ifndef __GradSoft_vector_INCLUDED
#include <vector>
#ifndef __GradSoft_vector_INCLUDED
#define __GradSoft_vector_INCLUDED
#endif
#endif

class SzArray
{

protected:

	std::vector<char *> strzs_;

public:

	SzArray() {}
	SzArray( const char*, const char );
	
	~SzArray() 
	{
		//
		// Memory must be allocateed by operator new
		//
		for(unsigned int i=0; i<strzs_.size(); i++)
			delete[] strzs_[i];
	}

public:

	//
	// get vector element
	//
	const char* get_sz_at( int i )  const
	{ return strzs_[i]; }

	char*       get_sz_at(int i)
	{ return strzs_[i]; }

	char*  operator []( int i )
	{ return strzs_[i]; }

	const char* operator []( int i ) const
	{ return strzs_[i]; }
	
	/**
	 * get size
	 */
	size_t size() const 
	{ return strzs_.size(); }

	/**
     * in: (prefix, suffix, delimiter)
	 * example: (["a","b","c"], "a", "z", ',') -> "aaz,abz,acz"
	 */
        char* generate_list(const char* prefix, const char* suffix, 
                            const char* delimiter);

	//
    // in: (sz_array, suffix)
    // incoming array must be the same size with stored
    // example: (["a", "z", 'p'], "=") -> ["a=a", "b=z", "c=p"]
	// where ["a","b","c"] is stored in *this
    //
	void  reduct_assigment(const SzArray&, const char*);

	//
	// append element to sz_array
	//
	void append_sz_nodup(char*);

	void append_sz_dup(const char*);

	//
	// append vector to sz_array
	//
	void append_szarray(const SzArray&);

private:

	SzArray& operator = (const SzArray& );

	SzArray(const SzArray&);

	
};

#endif
