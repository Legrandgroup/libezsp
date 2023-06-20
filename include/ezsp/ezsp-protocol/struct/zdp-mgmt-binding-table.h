/**
 * @file zdp-mgmt-binding-table.h
 *
 * @brief Handles decoding of a bindings table
 */
#pragma once

#include <iomanip>

namespace NSEZSP {

class LIBEXPORT MgmtBindRsp {
	public:
	
		/**
		* @brief Default constructor
		*
		* Constructor without arguments
		*/
		explicit MgmtBindRsp() :
			SrcAddr(),
			Endpoint(0),
			Cluster(0),
			DstAddrMode(0),
			DstAddrEUI64(),
			DstAddrNodeId(0),
			DstEndpoint(0)	
		{};

		/**
		* @brief Dump this instance as a string
		*
		* @return The resulting string
		*/
		std::string String() const{
			std::stringstream buf;
			buf << "{ [ SrcAddr : ";
			for(uint8_t i=0; i < 8; i++) {
				if(i != 7){
					buf << std::hex << std::setw(2) << std::setfill('0') << unsigned(this->SrcAddr[i]) << ":";
				}else{
					buf << std::hex << std::setw(2) << std::setfill('0') << unsigned(this->SrcAddr[i]);
				}
			}
			buf << " ]" <<  "[ SrcEndpoint : 0x" << std::hex << std::setw(2) << std::setfill('0') << unsigned(this->Endpoint) << " ]"
				<<  "[ Cluster : 0x" << std::hex << std::setw(4) << std::setfill('0') << unsigned(this->Cluster) << " ]"
				<<  "[ DstAddrMode : 0x" << std::hex << std::setw(2) << std::setfill('0') << unsigned(this->DstAddrMode) << " ]"
				<<  "[ DstAddr : ";
			if(this->DstAddrNodeId != 0x0000){
				buf << "0x" << std::hex << std::setw(4) << std::setfill('0') << unsigned(this->DstAddrNodeId) << " ] }" << std::endl;
			}else{
				for(uint8_t i=0; i < 8; i++) {
					if(i != 7){
						buf << std::hex << std::setw(2) << std::setfill('0') << unsigned(this->DstAddrEUI64[i]) << ":";
					}else{
						buf << std::hex << std::setw(2) << std::setfill('0') << unsigned(this->DstAddrEUI64[i]);
					}
				}
				buf << " ]" << "[ DstEndpoint : 0x" << std::hex << std::setw(2) << std::setfill('0') << unsigned(this->DstEndpoint) << " ] }" << std::endl;
			}

			return buf.str();
		}

		//Setter

		void setSrcAddr(NSEZSP::EmberEUI64 SrcAddr) {
			this->SrcAddr = SrcAddr;
		}

		void setEndpoint(uint8_t Endpoint) {
			this->Endpoint = Endpoint;
		}

		void setCluster(uint16_t Cluster) {
			this->Cluster = Cluster;
		}

		void setDstAddrMode(uint8_t DstAddrMode) {
			this->DstAddrMode = DstAddrMode;
		}

		void setDstAddrEUI64(NSEZSP::EmberEUI64 DstAddr) {
			this->DstAddrEUI64 = DstAddr;
		}

		void setDstAddrNodeId(NSEZSP::EmberNodeId DstAddr) {
			this->DstAddrNodeId = DstAddr;
		}

		void setDstEndpoint(uint8_t DstEndpoint) {
			this->DstEndpoint = DstEndpoint;
		}

		//Getter

		NSEZSP::EmberEUI64 getSrcAddr() const{
			return SrcAddr;
		}

		uint8_t getEndpoint() const{
			return Endpoint;
		}

		uint16_t getCluster() const{
			return Cluster;
		}

		uint8_t getDstAddrMode() const{
			return DstAddrMode;
		}

		NSEZSP::EmberEUI64 getDstAddrEUI64() const{
			return DstAddrEUI64;
		}

		NSEZSP::EmberNodeId getDstAddrNodeId() const{
			return DstAddrNodeId;
		}

		uint8_t getDstEndpoint() const{
			return DstEndpoint;
		}


	private:
		NSEZSP::EmberEUI64 SrcAddr;
		uint8_t Endpoint;
		uint16_t Cluster;
		uint8_t DstAddrMode;
		NSEZSP::EmberEUI64 DstAddrEUI64;
		NSEZSP::EmberNodeId DstAddrNodeId = 0x0000;
		uint8_t DstEndpoint;	

};

}