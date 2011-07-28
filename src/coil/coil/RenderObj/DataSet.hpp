/*  dynamo:- Event driven molecular dynamics simulator 
    http://www.marcusbannerman.co.uk/dynamo
    Copyright (C) 2011  Marcus N Campbell Bannerman <m.bannerman@gmail.com>

    This program is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 3 as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include <coil/RenderObj/RenderObj.hpp>
#include <magnet/GL/buffer.hpp>
#include <vector>

namespace coil {
  /*! \brief This class encapsulates attributes (data) associated with
   * some topology.
   *
   * This class is the primary communication interface between a
   * simulation and the coil library. After the visualizer is
   * initialised, all data to be rendered should be passed through
   * this class.
   * 
   * The topology may be a collection of points or cells and the data
   * may be ordinates (positions of the points), extensive properties
   * (like the mass) or intensive properties (like the density). Some
   * data is scalar (like the temperature) and some data will have
   * several components per value (e.g. vector quantities like the
   * velocity).
   *
   * An important implementation note on Attributes is that they're
   * initialised on access. This is to facilitate the main thread
   * adding attributes after the GL threads initialisation phase.
   */
  class Attribute
  {
  public:
    enum AttributeType { 
      INTENSIVE, //!< Intensive property (e.g., Temperature, density)
      EXTENSIVE, //!< Extensive property (e.g., mass, momentum)
      COORDINATE //!< A special attribute which specifies the location of the attribute.
    };

    inline Attribute(size_t N, AttributeType type = EXTENSIVE, size_t components = 1):
      _hostData(N * components),
      _components(components),
      _type(type),
      _references(0)
    {}
    
    inline Attribute() {}

    /*! \brief Releases the OpenGL resources of this object.
     */
    inline void deinit() { _glData.deinit(); }

    /*! \brief Returns the GL buffer associated with the Attribute
     * data.
     */
    inline magnet::GL::Buffer<GLfloat>& getBuffer() { return _glData; }

    /** @name The host code interface. */
    /**@{*/
    
    /*! \brief Returns a reference to the host-cache of the Attribute
     * data.
     *
     * The Attribute data may be directly updated by the host program,
     * but flagNewData() must be called for the update to take effect.
     */
    inline std::vector<GLfloat>& getData() { return _hostData; }
    
    /*! \brief Marks that the data in the buffer has been updated, and
     * should be uploaded to the GL system.
     *
     * This function just inserts a callback in the GL system to
     * reinitialise the Attribute.
     */
    inline void flagNewData()
    { _glData.getContext().queueTask(magnet::function::Task::makeTask(&Attribute::initGLData, this)); }

    /*! \brief Test if the attribute is in use and should be
     * updated. 
     */
    inline bool active() const { return _references; }

    /*! \brief Returns the number of elements.
     */
    inline size_t size() const { return _hostData.size() / _components; }

    inline size_t getNComponents() const { return _components; }

    /**@}*/

  protected:
    void initGLData() { _glData.init(_hostData); }

    /*! \brief The OpenGL representation of the attribute data.
     *
     * There are N * _components floats of attribute data.
     */
    magnet::GL::Buffer<GLfloat> _glData;
    
    /*! \brief A host side cache of \ref _glData.
     *
     * This is used as a communication buffer, both when the host
     * program is writing into coil, and when coil passes the data
     * into OpenGL.
     */
    std::vector<GLfloat> _hostData;

    //! \brief The number of components per value.
    size_t _components;

    //! \brief The type of data stored in this Attribute.
    AttributeType _type;

    /*! \brief The number of glyphs, filters and other render objects
     * currently using this type.
     */
    size_t _references;
  };

  class DataSetChild: public RenderObj
  {
  public:
    virtual Gtk::TreeModel::iterator addViewRows(RenderObjectsGtkTreeView& view, Gtk::TreeModel::iterator&) = 0;
    
  protected:
  };
  
  /*! \brief A container class for a collection of \ref Attribute
   * instances forming a dataset, and any active filters/glyphs or any
   * other type derived from \ref DataSetChild.
   */
  class DataSet: public RenderObj, public std::map<std::string, Attribute>
  {
  public:
    DataSet(std::string name, size_t N): 
      RenderObj(name), 
      _context(NULL),
      _N(N) {}
    
    virtual void init(const std::tr1::shared_ptr<magnet::thread::TaskQueue>& systemQueue);

    virtual void deinit();

    /** @name The host code interface. */
    /**@{*/

    /*! \brief Add an Attribute to the DataSet.
     *
     * \param name The name of the Attribute.
     * \param type The type of the attribute.
     * \param components The number of components per value of the attribute.
     */
    void addAttribute(std::string name, Attribute::AttributeType type, size_t components);

    /*! \brief Looks up an attribute by its name.
     */
    inline Attribute& operator[](const std::string& name)
    {
      iterator iPtr = find(name);
      if (iPtr == end())
	M_throw() << "No attribute named " << name << " in Data set";
      
      return iPtr->second;
    }

    /**@}*/
        
    virtual void clTick(const magnet::GL::Camera& cam) 
    {
      for (std::vector<DataSetChild>::iterator iPtr = _children.begin();
	   iPtr != _children.end(); ++iPtr)
	iPtr->clTick(cam);
    }

    virtual void glRender(magnet::GL::FBO& fbo, const magnet::GL::Camera& cam)
    {
      for (std::vector<DataSetChild>::iterator iPtr = _children.begin();
	   iPtr != _children.end(); ++iPtr)
	iPtr->glRender(fbo, cam);
    }

    virtual Gtk::TreeModel::iterator addViewRows(RenderObjectsGtkTreeView& view)
    {
      Gtk::TreeModel::iterator iter = RenderObj::addViewRows(view);
      
      for (std::vector<DataSetChild>::iterator iPtr = _children.begin();
	   iPtr < _children.end(); ++iPtr)
	iPtr->addViewRows(view, iter);

      return iter;
    }

    virtual void showControls(Gtk::ScrolledWindow* win);

  protected:
    magnet::GL::Context* volatile _context;
    std::auto_ptr<Gtk::VBox> _gtkOptList;
    size_t _N;
    std::vector<DataSetChild> _children;

    void initGtk();

    void rebuildGui();

    struct ModelColumns : Gtk::TreeModelColumnRecord
    {
      ModelColumns()
      { add(name); add(components); add(type); }
      
      Gtk::TreeModelColumn<Glib::ustring> name;
      Gtk::TreeModelColumn<size_t> components;
      Gtk::TreeModelColumn<Attribute::AttributeType> type;
    };
    
    std::auto_ptr<ModelColumns> _attrcolumns;
    Glib::RefPtr<Gtk::TreeStore> _attrtreestore;
    std::auto_ptr<Gtk::TreeView> _attrview;
  };
}