/*  StrUtil.cpp
 *
 *  Copyright (C) 2013  Jim Evins <evins@snaught.com>
 *
 *  This file is part of gLabels-qt.
 *
 *  gLabels-qt is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  gLabels-qt is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gLabels-qt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "StrUtil.h"

#include <cmath>


namespace
{
	const double FRAC_EPSILON = 0.00005;
	const double denom[]        = {  1.0,  2.0, 3.0,  4.0,  8.0,  16.0,  32.0,  0.0 };
	const char*  denom_string[] = { "1", "₂", "₃", "₄", "₈", "₁₆", "₃₂", NULL };
	const char*  num_string[]   = {  "⁰",  "¹",  "²",  "³",  "⁴",  "⁵",  "⁶",  "⁷",  "⁸",  "⁹",
					 "¹⁰", "¹¹", "¹²", "¹³", "¹⁴", "¹⁵", "¹⁶", "¹⁷", "¹⁸", "¹⁹",
					 "²⁰", "²¹", "²²", "²³", "²⁴", "²⁵", "²⁶", "²⁷", "²⁸", "²⁹",
					 "³⁰", "³¹" };
}


namespace libglabels
{

	namespace StrUtil
	{

		QString &formatFraction( double x )
		{
                        int i;
                        double product, remainder;

                        for ( i=0; denom[i] != 0.0; i++ )
                        {
                                product = x * denom[i];
                                remainder = fabs(product - ((int)(product+0.5)));
                                if ( remainder < FRAC_EPSILON ) break;
                        }

                        if ( denom[i] == 0.0 )
                        {
                                /* None of our denominators work. */
                                return QString().sprintf( "%.5g", x );
                        }
                        if ( denom[i] == 1.0 )
                        {
                                /* Simple integer. */
                                return QString().sprintf( "%.0f", x );
                        }
                        int n = (int)( x * denom[i] + 0.5 );
                        int d = (int)denom[i];
                        if ( n > d )
                        {
                                return QString().sprintf( "%d%s/%s", (n/d), num_string[n%d], denom_string[i] );
                        }
                        else
                        {
                                return QString().sprintf( "%s/%s", num_string[n%d], denom_string[i] );
                        }
		}


		QString spanDigits( const QString &s, int *i )
		{
			QString chunk;

			for ( int j = *i; (j < s.size()) && s.at(j).isNumber(); j++ )
			{
				chunk.append( s.at(j) );
				*i = j+1; /* only advance i, if character is used. */
			}

			return chunk;
		}


		QString spanNonDigits( const QString &s, int *i )
		{
			QString chunk;

			for ( int j = *i; (j < s.size()) && !s.at(j).isNumber(); j++ )
			{
				chunk.append( s.at(j) );
				*i = j+1; /* only advance i, if character is used. */
			}

			return chunk;
		}


		/**
		 * Compare part names
		 * @s1: string to compare with s2.
		 * @s2: string to compare with s1.
		 *
		 * Compare two strings representing part names or numbers.  This function
		 * uses a natural sort order:
		 *
		 *  - Ignores case.
		 *
		 *  - Strings are divided into chunks (numeric and non-numeric)
		 *
		 *  - Non-numeric chunks are compared character by character
		 *
		 *  - Numerical chunks are compared numerically, so that "20" precedes "100".
		 *
		 *  - Comparison of chunks is performed left to right until the first difference
		 *    is encountered or all chunks evaluate as equal.
		 *
		 * Numeric chunks are converted to 64 bit unsigned integers for comparison,
		 * so the behaviour may be unpredictable for numeric chunks that exceed
		 * 18446744073709551615.
		 *
		 * Returns: 0 if the strings match, a negative value if s1 < s2,
		 *          or a positive value if s1 > s2.
		 *
		 */
		int comparePartNames( const QString &s1, const QString &s2 )
		{
			if ( s1 == s2 ) return 0;
			if ( s1 == "" ) return -1;
			if ( s2 == "" ) return 1;

			QString folded_s1 = s1.toUpper();
			QString folded_s2 = s2.toUpper();

			int i1 = 0;
			int i2 = 0;
			int result = 0;
			bool done = false;

			while ( (result == 0) && !done )
			{
				QString chunk1, chunk2;
				bool   isnum1, isnum2;

				if ( folded_s1.at( i1 ).isNumber() )
				{
					chunk1 = spanDigits( folded_s1, &i1 );
					isnum1 = true;
				}
				else
				{
					chunk1 = spanNonDigits( folded_s1, &i1 );
					isnum1 = false;
				}
                
				if ( folded_s2.at( i2 ).isNumber() )
				{
					chunk2 = spanDigits( folded_s2, &i2 );
					isnum2 = true;
				}
				else
				{
					chunk2 = spanNonDigits( folded_s2, &i2 );
					isnum2 = false;
				}


				if ( ( chunk1 == "" ) && ( chunk2 == "" ) )
				{
					/* Case 1: Both are empty. */
					done = true;
				}
				else if ( isnum1 && isnum2 )
				{
					/* Case 2: They both contain numbers */
					qlonglong n1 = chunk1.toLongLong();
					qlonglong n2 = chunk2.toLongLong();

					if ( n1 < n2 ) result = -1;
					else if ( n1 > n2 ) result =  1;
				}
				else
				{
					/* Case 3: One or both do not contain numbers */
					if ( chunk1 < chunk2 ) result = -1;
					else if( chunk1 > chunk2 ) result = 1;
				}

			}

			return result;
		}



	}


}

