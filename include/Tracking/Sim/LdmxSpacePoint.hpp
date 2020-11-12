/**
 *@brief Implementation of an external space point
 *@author PF, SLAC
 */


//TODO:: Covariance?!

#include <cmath>

namespace ldmx {

    class LdmxSpacePoint  {
        
    public : 
        LdmxSpacePoint(float x, float y,
                       float z, float t,
                       int layer) {
            
            m_x = x;
            m_y = y;
            m_z = z;
            m_t = t;
            m_r = std::sqrt(m_x*m_x + m_y*m_y);
            m_layer = layer;
            m_id = -999;
        }
        
        LdmxSpacePoint(float x,float y,float z,float t,int layer,int id) {
            m_x = x;
            m_y = y;
            m_z = z;
            m_t = t;
            m_r = std::sqrt(m_x*m_x + m_y*m_y);
            m_layer = layer;
            m_id = id;
        }
        
        float x() const {return m_x;}
        float y() const {return m_y;}
        float z() const {return m_z;}
        float t() const {return m_t;}
        float r() const {return m_r;}
        int   layer() const {return m_layer;}
        int id() const {return m_id;}
        
        
    private:
        
        float m_x;
        float m_y;
        float m_z;
        float m_t;
        float m_r;
        int   m_id;
        int   m_layer;
        
    };    
    
        

    


}
 
