///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///

#pragma once

#include "sgxutility.h"
#include <vector>

//This class is meant for enforcing unique keys like map or unordered_map where insertion order is important and
//the number of values in the container is small

namespace SynGlyphX {

	template <typename Key, typename Value>
	class InsertionOrderMap : public std::vector<std::pair<Key, Value>> {

	public:
		typedef std::pair<Key, Value> KeyValuePair;

		InsertionOrderMap() : std::vector<std::pair<Key, Value>>() {}
		InsertionOrderMap(const InsertionOrderMap& vector) : std::vector<std::pair<Key, Value>>(vector) {}
		virtual ~InsertionOrderMap() {}

		InsertionOrderMap& operator=(const InsertionOrderMap& vector) {
			
			std::vector<std::pair<Key, Value>>::operator=(vector); 
			return *this; 
		}

		bool operator==(const InsertionOrderMap& vector) const {

			return (size() == vector.size() && std::equal(begin(), end(), vector.begin()));
		}

		bool operator!=(const InsertionOrderMap& vector) const {

			return !operator==(vector);
		}

		bool HasKey(const Key& key) const {

			for (unsigned int i = 0; i < size(); ++i) {

				if (this->operator[i].first == key) {

					return true;
				}
			}

			return false;
		}

		void Insert(const Key& key, const Value& value) {

			push_back(std::pair<Key, Value>(key, value));
		}

		void Insert(unsigned int index, const KeyValuePair& keyValuePair) {

			std::vector<KeyValuePair>::iterator iT = begin();
			std::advance(iT, index + 1);
			this->insert(iT, keyValuePair);
		}

		void Erase(const Key& key) {

			for (std::vector<std::pair<Key, Value>>::iterator iT = begin(); iT != end(); ++iT) {

				if (iT->first == key) {

					this->erase(iT);
					break;
				}
			}
		}

		void Erase(unsigned int index) {

			std::vector<std::pair<Key, Value>>::iterator iT = begin();
			std::advance(iT, index);
			this->erase(iT);
		}
	};

} //namespace SynGlyphX

//#pragma once