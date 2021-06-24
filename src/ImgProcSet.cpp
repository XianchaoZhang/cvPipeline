#include "ImgProcSet.h"
#include <typeinfo>
#include <time.h>
#if defined(_MSC_VER)
#include <stdexcept>
#endif

namespace cvPipeline{
    namespace Filter{
        ///////////////////////////////////////////////////////////////////////
        // class ImgProcSet
        
        IMPLEMENT_CVFILTER(ImgProcSet);
        // cvPipeline::Filter::ImageProcessor* ImgProcSet::createFilter(){return new ImgProcSet;}
        // void ImgProcSet::entryFilter(){
        //     cvPipeline::Filter::ImageProcessor::entryFilter(ImgProcSet, &ImgProcSet::createFilter);
        // }
        // cvPipeline::Filter::InitCallback<ImgProcSet, ImgProcSet::entryFilter> ImgProcSet _entry_static_;
        
        ImgProcSet::ImgProcSet():lastAutoProcessor(0)
        {
            
        }
        ImgProcSet::~ImgProcSet(){
            std::vector<ImageProcessor*>::iterator dyn_filter = dyn_filters.begin();
            while(dyn_filter != dyn_filters.end()){
#ifdef _DEBUG
                std::cout << "delete filter " << typeid(**dyn_filter).name() << std::endl;
#endif
                delete *dyn_filter;
                *dyn_filter = 0;
                dyn_filter++;
            }
        }
        
        ImageProcessor& ImgProcSet::add(
            std::string filter_class_name,
            std::string filter_instance_name,
            bool autoBind)
        {
            ImageProcessor* filter = ImageProcessor::createFilter(filter_class_name);
            if (filter != 0){
                dyn_filters.push_back(filter);
                name_to_filter[filter_instance_name] = filter;
                filter->setName(getName()+"."+filter_instance_name);
                add(*filter, autoBind);
            }else{
                std::cerr << "ImageProcessor \"" << filter_class_name << "\" is not exist in "
                    << getName() << " instance of class " << std::string(typeid(*this).name()) << "."
                    << std::endl;
#if defined(_MSC_VER)
                //throw new std::exception("PropertyNotFound");
#else
                throw new std::runtime_error("PropertyNotFound");
#endif
            }
            return *filter;
        }
        ImageProcessor& ImgProcSet::add(ImageProcessor& proc, bool autoBind)
        {
            if(autoBind){
                std::list<ImageProcessor*>::reverse_iterator back = procs.rbegin();
                if(back != procs.rend()){
                    ImageProcessor* last = *back;
                    *last >> proc;
                }else{
                    proc.setInputMat(getInputMat());
                }
                refOutputMat() = proc.getOutputMat();
                lastAutoProcessor = &proc;
            }
            procs.push_back(&proc);
            return proc;
        }
        void ImgProcSet::setParam(
            std::string filter_instance_name,
            std::string parameter_name,
            int value)
        {
            (*this)[filter_instance_name].property(parameter_name, value);
        }
        ImageProcessor& ImgProcSet::operator[](const std::string& filter_instance_name)
        {
            ImageProcessor* filter = 0;
            if(name_to_filter.count(filter_instance_name) >0 ){
                filter = name_to_filter[filter_instance_name];
            }else{
                std::cerr << "ImageProcessor named \"" << filter_instance_name << "\" is not exist in "
                        << getName() << " instance of class " << std::string(typeid(*this).name()) << "."
                        << std::endl;
#if defined(_MSC_VER)
				//throw new std::exception("ProcessorInstanceNotFound");
#else
                throw new std::runtime_error("ProcessorInstanceNotFound");
#endif
            }
            return *filter;
        }

        ImageProcessor& ImgProcSet::getLastProc()
        {
            std::list<ImageProcessor*>::reverse_iterator last = procs.rbegin();
            return *(*last);
        }
        
        void ImgProcSet::execute()
        {
            if(isEnable()){
                std::list<ImageProcessor*>::iterator proc = procs.begin();
                while(proc != procs.end()){
                    if((*proc)->isEnable()){
                        try{
                            (*proc)->execute();
                        }
                        catch(...){
                            std::cerr << "exception caught in " << getName()
                                << "from " << (*proc)->getName() << "."
                                << std::endl;
                        }
                    }
                    proc++;
                }
            }
            if(lastAutoProcessor != 0){
                setOutputMat(lastAutoProcessor->getOutputMat());
            }
        }

        void ImgProcSet::putMarkdown(std::ostream& stream)
        {
            ImageProcessor::putMarkdown(stream);
            stream << "----" << std::endl;
            stream << std::endl;
            std::list<ImageProcessor*>::iterator proc = procs.begin();
            for(; proc!=procs.end(); proc++){
                (*proc)->putMarkdown(stream);
            }
        }
        
        using namespace pugi;
        // 从 xml 文件创建处理器管道
        bool ImgProcSet::loadXml(const std::string& filename)
        {
            pugi::xml_document doc;
            pugi::xml_parse_result rst = doc.load_file(filename.c_str());
            if(rst.status != status_ok){
                return false;
            }
            pugi::xml_node root_node = doc.document_element();
#ifdef _DEBUG
            std::cerr << "[ImgProcSet::loadXml]" << root_node.name() << std::endl;
#endif
            pugi::xml_attribute root_name_attr = root_node.attribute("name");
            setName(root_name_attr.as_string());
            
            xpath_node_set processor_node_set = root_node.select_nodes("Processor");
            xpath_node_set::const_iterator processor = processor_node_set.begin();
            while(processor != processor_node_set.end()){
                addProcessorByXmlNode(processor->node());
                processor++;
            }
            return true;
        }
        
        void ImgProcSet::addProcessorByXmlNode(pugi::xml_node processor)
        {
            // 属性类名称
            xml_attribute attr_proc_class = processor.attribute("class");
            // 属性实例名称
            xml_attribute attr_proc_name = processor.attribute("name");
#ifdef _DEBUG
            std::cerr << "[ImgProcSet::addProcessorByXmlNode] " "Processor "
                      << attr_proc_class.name() << " = " << attr_proc_class.as_string() << ", "
                      << attr_proc_name.name() << " = " << attr_proc_name.as_string() << std::endl;
#endif
            // Attr autoBind
            xml_attribute attr_bind = processor.attribute("autoBind");
            
            std::string autoBindAttrName = std::string(attr_bind.name());
            bool autoBind = true;
            if(autoBindAttrName == ""){
#ifdef _DEBUG
            std::cerr << "property autoBind not specified. default=true" << std::endl;
#endif
            }else{
                autoBind = attr_bind.as_bool();
            }
#ifdef _DEBUG
            std::cerr << "property autoBind = \"" << autoBind << "\"" << std::endl;
#endif

            // 创建并添加处理器
            ImageProcessor& proc = add(
                attr_proc_class.as_string(),
                attr_proc_name.as_string(),
                autoBind);
            // 属性
            xpath_node_set property_node_set = processor.select_nodes("Property");
            xpath_node_set::const_iterator property = property_node_set.begin();
            while(property != property_node_set.end()){
                proc.setPropertyByXmlNode(property->node());
                property++;
            }
            
            // 输入图形
            xpath_node_set input_node_set = processor.select_nodes("Input");
            xpath_node_set::const_iterator input = input_node_set.begin();
            while(input != input_node_set.end()){
                setInputMatByXmlNode(proc, input->node());
                input++;
            }
        }
        
        void ImgProcSet::setInputMatByXmlNode(ImageProcessor& imageProcessor, pugi::xml_node input)
        {
            std::string srcProcName = std::string(input.attribute("from").as_string());
            std::string dstInMatName = std::string(input.attribute("to").as_string());
            
            FilterInput fi = imageProcessor.input(dstInMatName);
            (*this)[srcProcName] >> fi;
            //(*this)[srcProcName] >> imageProcessor.input(dst
        }
        
        void ImageProcessor::setPropertyByXmlNode(pugi::xml_node property)
        {
            xml_attribute attr_prop_name = property.attribute("name");
            xml_attribute attr_prop_value = property.attribute("value");
#ifdef _DEBUG
            std::cerr << "[ImageProcessor::setPropertyByXmlNode] " << "property "
                    << attr_prop_name.name() << " = " << attr_prop_name.as_string() << ", "
                    << attr_prop_value.name() << " = " << attr_prop_value.as_string() << std::endl;
#endif
            setPropertyAsString(attr_prop_name.as_string(), attr_prop_value.as_string());
        }
    }
}