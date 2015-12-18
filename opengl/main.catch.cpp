#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ctemplate/template.h>

SCENARIO("第一个ctemplate测试")
{
    GIVEN("第一个ctemplate测试")
    {
        auto example_tpl = u8R"W83JmYmCH8BmqVCo(
        <html>
        <head>
        <title>ctemplate示例模板</title>
        </head>
        
        <body>
        {{table1_name}}
        <table>
        {{#TABLE1}}
        <tr>
        <td>{{field1}}</td>
        <td>{{field2}}</td>
        <td>{{field3}}</td>
        </tr>
        {{/TABLE1}}
        </table>
        {{>INCLUDED_TEMPLATE}}
        </body>
        </html>
        )W83JmYmCH8BmqVCo";
        
        ctemplate::TemplateDictionary dict("example");
        dict.SetValue("table1_name", "表格名字");
        for (int i=0; i<3; i++)
        {
            auto x = dict.AddSectionDictionary("TABLE1");
            x->SetFormattedValue("field1", "大熊猫 %d", i);
            x->SetFormattedValue("field2", "小熊猫 %d", i);
            x->SetFormattedValue("field3", "美熊猫 %d", i);
        }
        
        
        
        bool OK = ctemplate::StringToTemplateCache("example_tpl", example_tpl, ctemplate::DO_NOT_STRIP);
        REQUIRE(OK == true);
        
        std::string output;
        ctemplate::ExpandTemplate("example_tpl", ctemplate::DO_NOT_STRIP, &dict, &output);
        
        std::cout<<output<<std::endl;
    }
}