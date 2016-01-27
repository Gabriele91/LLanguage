//
//  l_compile_tree_to_js.cpp
//  LLanguage
//
//  Created by Gabriele on 27/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <compile_tree_to_js.h>

namespace l_language
{
    const char* code_js_header =
    R"JS_CODE(
    <!DOCTYPE html>
    <html>
    <head>
    <meta charset="UTF-8"/>
    </head>
    <style>
    
    .console
    {
    margin: 20px;
    }
    
    .console #header
    {
    width: 640px;
    color: #000;
        background-color: #CFCFCF;
    padding: 5px 20px 5px 20px;
        border-top-left-radius:  5px;
        border-top-right-radius: 5px;
        box-shadow: 10px 10px 5px #999999;
    }
    
    .console #text
    {
    width: 640px;
        min-height: 480px;
    color: #FFF;
        text-align: left;
        background-color: #000;
    padding: 20px;
        border-bottom-left-radius:  5px;
        border-bottom-right-radius: 5px;
        box-shadow: 10px 10px 5px #999999;
    }
    
    </style>
    <body>
    <center>
    <div class="console">
    <div id="header">CONSOLE</div>
    <div id="text"></div>
    <div>
    </center>
    </body>
    <script>
    
    function print()
    {
        function nl2br (str)
        {
            return (str + "").replace(/([^>\r\n]?)(\r\n|\n\r|\r|\n)/g, "$1" + "<br>" + "$2");
        }
        var console = document.getElementById("text");
        for (var i=0; i < arguments.length; ++i)
        {
            console.innerHTML += nl2br(arguments[i]);
        }
    }
    
    function len(value)
    {
        return value.length;
    }
    
    
    function push(v_array,value)
    {
        v_array.push(value);
    }
    
    function input()
    {
        if(arguments.length == 0) return prompt("inserisci");
        else                      return prompt(arguments[0]);
    }
    
    function to_int(value)
    {
        if(typeof value === 'string') return parseInt(value);
        else                          return value;
    }
    
    function to_string(value)
    {
        if(typeof value === 'string') return value;
        else                          return value+"";
    }
    
    )JS_CODE";
    
    const char* code_js_end =
    R"JS_CODE(
    </script>
    </html>
    )JS_CODE";
}