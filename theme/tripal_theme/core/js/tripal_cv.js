
if (Drupal.jsEnabled) {
  
   $(document).ready(function() {
     // any img object of class .tripal_cv_chart will have it's src attribute
     // populated when the document is ready by the following code.  The id
     // for the object must have a unique identifier that the calling tripal
     // module recognizes 
     tripal_cv_init_chart();

     // any div object of class .tripal_cv_tree will be populated with a 
     // CV brower tree when the document is ready.  The id for the object
     // must have a unique identifier that the calling tripal module
     // recognizes.
     tripal_cv_init_tree();

     $(window).scroll(function(){
       $('#tripal_cv_cvterm_info_box').animate(
           {top:$(window).scrollTop()+"px" },
           {queue: false, duration: 350}
        );
     });

     $("#tripal_cv_cvterm_info_box").hide();
   });

   //------------------------------------------------------------
   // FUNCTIONS
   //------------------------------------------------------------
   function tripal_cv_init_tree(){
     $(".tripal_cv_tree").attr("id", function(){
         var api = new jGCharts.Api();
         var tree_id = $(this).attr("id");
         var link = baseurl + "/";
         if(!isClean){
            link += "?q=";
         }
         link += 'tripal_cv_tree/' + tree_id; 
         tripal_startAjax();
         $.ajax({
            url: link,
            dataType: 'json',
            type: 'POST',
            success: function(data){  
                 vars = { 
                  cv : data[0],
                  tree_id : data[1],
                 }
                 init_tree(tree_id,vars);    
                 tripal_stopAjax();
            }
         });
      });
   }

   function tripal_cv_init_chart(){
     $(".tripal_cv_chart").attr("src", function(){
         var api = new jGCharts.Api();
         var chart_id = $(this).attr("id");
         var link = baseurl + "/";
         if(!isClean){
            link += "?q=";
         }
         link += 'tripal_cv_chart/' + chart_id;
         tripal_startAjax();
         $.ajax({
            url: link,
            dataType: 'json',
            type: 'POST',
            success: function(data){  
               src = api.make(data[0]);
               chart_id = data[1];
               $('#' + chart_id).attr('src',src);       
               tripal_stopAjax();
            }
         });
      });
   }
   // The Tripal CV module provides a CV term browser.  This function
   // initializes that browser.
   function tripal_cv_init_browser(options){
      // Get the cv_id from DOM
      var index = options.selectedIndex;
      var cv_id = options[index].value;
      var link = baseurl + '/tripal_cv_init_browser/' + cv_id;
      $.ajax({
         url:  link,
         dataType: 'json',
         type: 'POST',
         success: function(data){         
            $("#cv_browser").html(data.update);
            vars = { 
                cv : cv_id,
            }  
            init_tree('browser',vars);
         }
      });
      return false;
   }

   //------------------------------------------------------------
   // When a term in CV term tree browser is clicked this
   // function loads the term info into a box on the page.
   function tripal_cv_cvterm_info(cvterm_id,vars){
      var link = baseurl + "/";
      if(!isClean){
         link += "?q=";
      }
      link += 'tripal_cv_cvterm_info/' + cvterm_id + '?cv=' + vars.cv + '&tree_id=' + vars.tree_id;

      // Get the cv_id from DOM
      $.ajax({
         url: link,
         dataType: 'json',
         type: 'POST',
         success: function(data){         
            $("#tripal_cv_cvterm_info").html(data.update);
            $('#tripal_cv_cvterm_info_box').animate(
                {top:$(window).scrollTop()+"px" },
                {queue: false, duration: 350}
            );
            $("#tripal_cv_cvterm_info_box").show();
         }
      });
      return false;
   }
   //------------------------------------------------------------
   // This function initializes a CV term tree
   function init_tree(id,vars){
      var link = baseurl + "/";
      if(!isClean){
         link += "?q=";
      }
      link += 'tripal_cv_update_tree';
      var theme_link = baseurl + '/' + themedir + "/js/jsTree/source/themes/";
      $("#" + id).tree ({
        data    : {
          type    : "json", // ENUM [json, xml_flat, xml_nested, predefined]
          method  : "GET",        // HOW TO REQUEST FILES
          async   : true,        // BOOL - async loading onopen
          async_data : function (NODE) { 
             vars.term = $(NODE).attr("id") || "root";
             return vars
          }, // PARAMETERS PASSED TO SERVER
          url     : link,    // FALSE or STRING - url to document to be used (async or not)
          json    : false,        // FALSE or OBJECT if type is JSON and async is false - the tree dump as json
          xml     : false         // FALSE or STRING
        },
        ui      : {
           dots        : true,     // BOOL - dots or no dots
           rtl         : false,    // BOOL - is the tree right-to-left
           animation   : 30,        // INT - duration of open/close animations in miliseconds
           hover_mode  : true,     // SHOULD get_* functions chage focus or change hovered item
           scroll_spd  : 4,
           theme_path  : theme_link,    // Path to themes
           theme_name  : "classic",// Name of theme
        },

        rules   : {
           multiple    : false,    // FALSE | CTRL | ON - multiple selection off/ with or without holding Ctrl
           metadata    : false,    // FALSE or STRING - attribute name (use metadata plugin)
           type_attr   : "rel",    // STRING attribute name (where is the type stored if no metadata)
           multitree   : false,    // BOOL - is drag n drop between trees allowed
           createat    : "bottom", // STRING (top or bottom) new nodes get inserted at top or bottom
           use_inline  : false,    // CHECK FOR INLINE RULES - REQUIRES METADATA
           clickable   : "all",    // which node types can the user select | default - all
           renameable  : false,    // which node types can the user select | default - all
           deletable   : false,    // which node types can the user delete | default - all
           creatable   : false,    // which node types can the user create in | default - all
           draggable   : "none",   // which node types can the user move | default - none | "all"
           dragrules   : "all",    // what move operations between nodes are allowed | default - none | "all"
           drag_copy   : false,    // FALSE | CTRL | ON - drag to copy off/ with or without holding Ctrl
           droppable   : [],
           drag_button : "left"
        },

        callback    : {             // various callbacks to attach custom logic to
           // before focus  - should return true | false
           beforechange: function(NODE,TREE_OBJ) { return true },
           beforeopen  : function(NODE,TREE_OBJ) { return true },
           beforeclose : function(NODE,TREE_OBJ) { return true },
           // before move   - should return true | false
           beforemove  : function(NODE,REF_NODE,TYPE,TREE_OBJ) { return true }, 
           // before create - should return true | false
           beforecreate: function(NODE,REF_NODE,TYPE,TREE_OBJ) { return true }, 
           // before rename - should return true | false
           beforerename: function(NODE,LANG,TREE_OBJ) { return true }, 
           // before delete - should return true | false
           beforedelete: function(NODE,TREE_OBJ) { return true }, 

           onJSONdata  : function(DATA,TREE_OBJ) { return DATA; },
           onselect    : function(NODE,TREE_OBJ) { tripal_cv_cvterm_info( $(NODE).attr("id"),vars )},                  // node selected
           ondeselect  : function(NODE,TREE_OBJ) { },                  // node deselected
           onchange    : function(NODE,TREE_OBJ) { },                  // focus changed
           onrename    : function(NODE,LANG,TREE_OBJ,RB) { },              // node renamed ISNEW - TRUE|FALSE, current language
           onmove      : function(NODE,REF_NODE,TYPE,TREE_OBJ,RB) { }, // move completed (TYPE is BELOW|ABOVE|INSIDE)
           oncopy      : function(NODE,REF_NODE,TYPE,TREE_OBJ,RB) { }, // copy completed (TYPE is BELOW|ABOVE|INSIDE)
           oncreate    : function(NODE,REF_NODE,TYPE,TREE_OBJ,RB) { }, // node created, parent node (TYPE is createat)
           ondelete    : function(NODE, TREE_OBJ,RB) { },                  // node deleted
           onopen      : function(NODE, TREE_OBJ) { },                 // node opened
           onopen_all  : function(TREE_OBJ) { },                       // all nodes opened
           onclose     : function(NODE, TREE_OBJ) { },                 // node closed
           error       : function(TEXT, TREE_OBJ) { },                 // error occured
           // double click on node - defaults to open/close & select
           ondblclk    : function(NODE, TREE_OBJ) { TREE_OBJ.toggle_branch.call(TREE_OBJ, NODE); TREE_OBJ.select_branch.call(TREE_OBJ, NODE); },
           // right click - to prevent use: EV.preventDefault(); EV.stopPropagation(); return false
           onrgtclk    : function(NODE, TREE_OBJ, EV) { },
           onload      : function(TREE_OBJ) { },
           onfocus     : function(TREE_OBJ) { },
           ondrop      : function(NODE,REF_NODE,TYPE,TREE_OBJ) {}
        }
      });
   }

 
}
