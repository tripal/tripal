//YCodaSlider.Base.css('../../include/css/id.css');
//YCodaSlider.Base.css('../../include/js/lib/ycodaslider-2.0.rc2/skins/base/style.css');
//YCodaSlider.Base.css('../../include/js/lib/ycodaslider-2.0.rc2/skins/base/desktop/desktop.css');
jQuery(window).ycodacss('../../include/js/lib/ycodaslider-3.0/ycodaslider-3.0.css');
//jQuery(document).ready(function() {
//    if(location.toString().indexOf("maxb.net") !== -1){
//    	jQuery('body').append('<div id="analytics" class="UA-258871-1"></div>');
//    }
//});
jQuery(window).bind("load", function() {
	
    var wh = window.innerHeight || jQuery(window).height();
    var ww = window.innerWidth || jQuery(window).width();
    wh = wh - 100;ww = ww - 180;
    
    jQuery("div#yslider-table")
    .ycodaslider({
         height  : wh,
         width: (ww-100), 
         scroll : true,
         tracking : true,
         tracking_pre : 'jgtable'
    });
    
    jQuery("div#yslider-docs")
    .ycodaslider({
         height  : wh, 
         scroll : true,
         tracking : true,
         tracking_pre : 'jgcharts'
    });
    
    jQuery(".h-code").click(function(){
         var ref = jQuery("div#yslider-code").get(0);
         if(!ref.loaded){
             jQuery("div#yslider-code")
             .ycodacode()
             .ycodaslider({
             	width: (ww-100), 
             	height  : wh,
             	scroll : true,
             	tracking : true,
         	    tracking_pre : 'jgcharts-code'
             });
             ref.loaded = true;
         }
    });

    jQuery(".ycodaslider").hide();
    jQuery(".handle").each(function(nr){
        var target = jQuery(this).attr("class").split(" ")[1].split("-")[1];
        jQuery(this).click(function(){
            jQuery(".ycodaslider").hide();
            jQuery("#yslider-" + target).toggle();
        });
    });
    jQuery("#yslider-docs").show();
    
    //utils
    jQuery("a.blank").attr("target", "_blank");
    
    //my chili wrapper
    jQuery("div.code").each(function(){
        var _c = jQuery(this).attr("class").split(" ")[1];
        var _t = html_encode(jQuery(this).html());
        jQuery(this).html("");
        jQuery('<pre><code class="'+_c+'">'+_t+'</code></pre>')
        .chili()
        .appendTo(this);
    });
    jQuery("code").chili();
    function html_encode(s) {
       var str = new String(s);
        str = str.replace("<!--", "");//first comment
        str = str.substr(0,str.length - 3);//last comment
        str = str.replace(/&/g, "&amp;");
        str = str.replace(/</g, "&lt;");
        str = str.replace(/>/g, "&gt;");
        str = str.replace(/"/g, "&quot;");
        
        return str;
    }

    //fix ie6
    if(jQuery.browser.msie && jQuery.browser.version < 7){
        alert("Get Firefox!");
        jQuery(".yslider-panelwrapper").css("padding","2");
        jQuery("#icone_sinistra img, #icone_destra img").each(function(){
            var gif = jQuery(this).attr("src").replace("png","gif");
            jQuery(this).attr("src", gif);//icone desktop
            //alert(gif);
        });
        jQuery(".yslider-navr, .yslider-navl").each(function(){
            var gif = jQuery(this).css("background-image").replace("png","gif");
            //console.log(gif);
            jQuery(this).css("background-image", gif);//icone slider
        });
    }
    //CHARTS DEMO
    
    //bar1
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]]}))
    .appendTo("#bar1");
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           size : '400x400'}))
    .appendTo("#bar2");
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           type : 'bhg'}))
    .appendTo("#bar3");
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           axis_labels : ['2008','2007','2006'], 
                           legend : ['serie1', 'serie2', 'serie3']}))
    .appendTo("#bar4");
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           bar_width : 10, bar_spacing : 10}))
    .appendTo("#bar5");

    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           colors : ['4b9b41','81419b','41599b']}))
    .appendTo("#bar6");  


    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           bg        : 'e0e0e0'//,
                           //bg_trasparency : 50
                           }))
    .appendTo("#bar7");
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           bg        : 'FFFFFF',
                           bg_offset : '000000',
                           bg_angle  : '45',//default 90   
                           bg_type   : 'gradient' //default solid
                           }))
    .appendTo("#bar8");  
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           bg        : 'FFFFFF',
                           bg_offset : 'e0e0e0',
                           bg_angle  : '0',//default 90   
                           bg_type   : 'stripes', //default solid
                           bg_width  : '10'//default 10 - min 10
                           }))
    .appendTo("#bar9");                   

    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           chbg        : 'FFFFFF',
                           chbg_offset : '4b9b41',
                           chbg_angle  : '45',//default 90   
                           chbg_type   : 'gradient' //default solid
                           }))
    .appendTo("#bar10");
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           title       : 'Bar Chart', //default false
                           title_color : 'a98147',
                           title_size  : 20 //default 10
                          }))
    .appendTo("#bar11");
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           grid        : true, //default false
                           grid_x      : 5,    //default 10
                           grid_y      : 5,    //default 10
                           grid_line   : 5,   //default 10
                           grid_blank  : 0    //default 0
                          }))
    .appendTo("#bar12");
    
    
    
    //stacked
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           type : 'bhs'}))
    .appendTo("#stacked1");
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           type : 'bvs'}))
    .appendTo("#stacked2");
    
    //line
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[105.7,97.9],[108.1,101.6],[110.7,102.9],[111.0,93.7],[110.0,89.8],[109.0,90.7],[107.5,93.0],[106.1,94.5],[104.3,91.9],[102.0,93.9],[102.8,93.6],[103.8,92.6],[102.9,94.0],[102.1,92.7],[100.6,96.0],[101.7,97.9],[101.8,105.0],[103.3,104.1],[104.0,105.1],[103.7,108.1],[108.4,108.4],[109.4,113.8],[112.0,109.1],[112.6,106.3],[115.5,106.7],[115.7,108.8],[114.7,118.8],[115.9,120.4],[116.2,115.9],[118.0,124.7],[123.3,126.5],[127.6,131.6],[130.3,134.0],[135.5,135.7],[138.2,126.4],[139.6,127.4],[145.1,131.0],[146.4,129.9],[147.1,133.7],[149.0,138.4],[150.3,141.0],[151.3,139.3],[153.4,145.3],[152.7,142.9],[152.9,129.2],[152.2,126.0],[151.9,124.8],[150.1,125.9],[148.2,118.9],[145.3,122.9],[142.9,127.7],[142.6,134.4],[144.0,138.5],[145.5,138.7],[147.2,141.8],[150.0,139.2],[153.8,145.6],[155.4,147.6],[157.0,157.9],[158.4,156.2],[162.8,153.9],[162.8,158.6],[164.7,166.3],[168.5,165.8]], 
                           type : 'lc'
                          }))
    .appendTo("#line1");

    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[105.7,97.9],[108.1,101.6],[110.7,102.9],[111.0,93.7],[110.0,89.8],[109.0,90.7],[107.5,93.0],[106.1,94.5],[104.3,91.9],[102.0,93.9],[102.8,93.6],[103.8,92.6],[102.9,94.0],[102.1,92.7],[100.6,96.0],[101.7,97.9],[101.8,105.0],[103.3,104.1],[104.0,105.1],[103.7,108.1],[108.4,108.4],[109.4,113.8],[112.0,109.1],[112.6,106.3],[115.5,106.7],[115.7,108.8],[114.7,118.8],[115.9,120.4],[116.2,115.9],[118.0,124.7],[123.3,126.5],[127.6,131.6],[130.3,134.0],[135.5,135.7],[138.2,126.4],[139.6,127.4],[145.1,131.0],[146.4,129.9],[147.1,133.7],[149.0,138.4],[150.3,141.0],[151.3,139.3],[153.4,145.3],[152.7,142.9],[152.9,129.2],[152.2,126.0],[151.9,124.8],[150.1,125.9],[148.2,118.9],[145.3,122.9],[142.9,127.7],[142.6,134.4],[144.0,138.5],[145.5,138.7],[147.2,141.8],[150.0,139.2],[153.8,145.6],[155.4,147.6],[157.0,157.9],[158.4,156.2],[162.8,153.9],[162.8,158.6],[164.7,166.3],[168.5,165.8]], 
                           type : 'lc',
                           fillarea : true //default false
                          }))
    .appendTo("#line2");

    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[105.7,97.9],[108.1,101.6],[110.7,102.9],[111.0,93.7],[110.0,89.8],[109.0,90.7],[107.5,93.0],[106.1,94.5],[104.3,91.9],[102.0,93.9],[102.8,93.6],[103.8,92.6],[102.9,94.0],[102.1,92.7],[100.6,96.0],[101.7,97.9],[101.8,105.0],[103.3,104.1],[104.0,105.1],[103.7,108.1],[108.4,108.4],[109.4,113.8],[112.0,109.1],[112.6,106.3],[115.5,106.7],[115.7,108.8],[114.7,118.8],[115.9,120.4],[116.2,115.9],[118.0,124.7],[123.3,126.5],[127.6,131.6],[130.3,134.0],[135.5,135.7],[138.2,126.4],[139.6,127.4],[145.1,131.0],[146.4,129.9],[147.1,133.7],[149.0,138.4],[150.3,141.0],[151.3,139.3],[153.4,145.3],[152.7,142.9],[152.9,129.2],[152.2,126.0],[151.9,124.8],[150.1,125.9],[148.2,118.9],[145.3,122.9],[142.9,127.7],[142.6,134.4],[144.0,138.5],[145.5,138.7],[147.2,141.8],[150.0,139.2],[153.8,145.6],[155.4,147.6],[157.0,157.9],[158.4,156.2],[162.8,153.9],[162.8,158.6],[164.7,166.3],[168.5,165.8]], 
                           type : 'lc',
                           fillarea : true, //default false
                           fillbottom : true //default false
                          }))
    .appendTo("#line3");

    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[105.7,97.9],[108.1,101.6],[110.7,102.9],[111.0,93.7],[110.0,89.8],[109.0,90.7],[107.5,93.0],[106.1,94.5],[104.3,91.9],[102.0,93.9],[102.8,93.6],[103.8,92.6],[102.9,94.0],[102.1,92.7],[100.6,96.0],[101.7,97.9],[101.8,105.0],[103.3,104.1],[104.0,105.1],[103.7,108.1],[108.4,108.4],[109.4,113.8],[112.0,109.1],[112.6,106.3],[115.5,106.7],[115.7,108.8],[114.7,118.8],[115.9,120.4],[116.2,115.9],[118.0,124.7],[123.3,126.5],[127.6,131.6],[130.3,134.0],[135.5,135.7],[138.2,126.4],[139.6,127.4],[145.1,131.0],[146.4,129.9],[147.1,133.7],[149.0,138.4],[150.3,141.0],[151.3,139.3],[153.4,145.3],[152.7,142.9],[152.9,129.2],[152.2,126.0],[151.9,124.8],[150.1,125.9],[148.2,118.9],[145.3,122.9],[142.9,127.7],[142.6,134.4],[144.0,138.5],[145.5,138.7],[147.2,141.8],[150.0,139.2],[153.8,145.6],[155.4,147.6],[157.0,157.9],[158.4,156.2],[162.8,153.9],[162.8,158.6],[164.7,166.3],[168.5,165.8]], 
                           type : 'lc',
                           fillarea : true, //default false
                           fillbottom : true, //default false
                           filltop : true //default false
                          }))
    .appendTo("#line4");     
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[105.7,97.9],[108.1,101.6],[110.7,102.9],[111.0,93.7],[110.0,89.8],[109.0,90.7],[107.5,93.0],[106.1,94.5],[104.3,91.9],[102.0,93.9],[102.8,93.6],[103.8,92.6],[102.9,94.0],[102.1,92.7],[100.6,96.0],[101.7,97.9],[101.8,105.0],[103.3,104.1],[104.0,105.1],[103.7,108.1],[108.4,108.4],[109.4,113.8],[112.0,109.1],[112.6,106.3],[115.5,106.7],[115.7,108.8],[114.7,118.8],[115.9,120.4],[116.2,115.9],[118.0,124.7],[123.3,126.5],[127.6,131.6],[130.3,134.0],[135.5,135.7],[138.2,126.4],[139.6,127.4],[145.1,131.0],[146.4,129.9],[147.1,133.7],[149.0,138.4],[150.3,141.0],[151.3,139.3],[153.4,145.3],[152.7,142.9],[152.9,129.2],[152.2,126.0],[151.9,124.8],[150.1,125.9],[148.2,118.9],[145.3,122.9],[142.9,127.7],[142.6,134.4],[144.0,138.5],[145.5,138.7],[147.2,141.8],[150.0,139.2],[153.8,145.6],[155.4,147.6],[157.0,157.9],[158.4,156.2],[162.8,153.9],[162.8,158.6],[164.7,166.3],[168.5,165.8]], 
                           type : 'lc',
                           lines: [[4,2,2],[6,3,3]]   
                          }))
    .appendTo("#line5");     
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[105.7,97.9],[108.1,101.6],[110.7,102.9],[111.0,93.7],[110.0,89.8],[109.0,90.7],[107.5,93.0],[106.1,94.5],[104.3,91.9],[102.0,93.9],[102.8,93.6],[103.8,92.6],[102.9,94.0],[102.1,92.7],[100.6,96.0],[101.7,97.9],[101.8,105.0],[103.3,104.1],[104.0,105.1],[103.7,108.1],[108.4,108.4],[109.4,113.8],[112.0,109.1],[112.6,106.3],[115.5,106.7],[115.7,108.8],[114.7,118.8],[115.9,120.4],[116.2,115.9],[118.0,124.7],[123.3,126.5],[127.6,131.6],[130.3,134.0],[135.5,135.7],[138.2,126.4],[139.6,127.4],[145.1,131.0],[146.4,129.9],[147.1,133.7],[149.0,138.4],[150.3,141.0],[151.3,139.3],[153.4,145.3],[152.7,142.9],[152.9,129.2],[152.2,126.0],[151.9,124.8],[150.1,125.9],[148.2,118.9],[145.3,122.9],[142.9,127.7],[142.6,134.4],[144.0,138.5],[145.5,138.7],[147.2,141.8],[150.0,139.2],[153.8,145.6],[155.4,147.6],[157.0,157.9],[158.4,156.2],[162.8,153.9],[162.8,158.6],[164.7,166.3],[168.5,165.8]],
                           axis_labels : ['01-03','02-03','03-03','04-03','05-03','06-03','07-03','08-03','09-03','10-03','11-03','12-03','01-04','02-04','03-04','04-04','05-04','06-04','07-04','08-04','09-04','10-04','11-04','12-04','01-05','02-05','03-05','04-05','05-05','06-05','07-05','08-05','09-05','10-05','11-05','12-05','01-06','02-06','03-06','04-06','05-06','06-06','07-06','08-06','09-06','10-06','11-06','12-06','01-07','02-07','03-07','04-07','05-07','06-07','07-07','08-07','09-07','10-07','11-07','12-07','01-08','02-08','03-08','04-08'],
                           axis_step : 10, 
                           type : 'lc'   
                          }))
    .appendTo("#line6");
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[105.7,97.9],[108.1,101.6],[110.7,102.9],[111.0,93.7],[110.0,89.8],[109.0,90.7],[107.5,93.0],[106.1,94.5],[104.3,91.9],[102.0,93.9],[102.8,93.6],[103.8,92.6],[102.9,94.0],[102.1,92.7],[100.6,96.0],[101.7,97.9],[101.8,105.0],[103.3,104.1],[104.0,105.1],[103.7,108.1],[108.4,108.4],[109.4,113.8],[112.0,109.1],[112.6,106.3],[115.5,106.7],[115.7,108.8],[114.7,118.8],[115.9,120.4],[116.2,115.9],[118.0,124.7],[123.3,126.5],[127.6,131.6],[130.3,134.0],[135.5,135.7],[138.2,126.4],[139.6,127.4],[145.1,131.0],[146.4,129.9],[147.1,133.7],[149.0,138.4],[150.3,141.0],[151.3,139.3],[153.4,145.3],[152.7,142.9],[152.9,129.2],[152.2,126.0],[151.9,124.8],[150.1,125.9],[148.2,118.9],[145.3,122.9],[142.9,127.7],[142.6,134.4],[144.0,138.5],[145.5,138.7],[147.2,141.8],[150.0,139.2],[153.8,145.6],[155.4,147.6],[157.0,157.9],[158.4,156.2],[162.8,153.9],[162.8,158.6],[164.7,166.3],[168.5,165.8]], 
                           type : 'ls'   
                          }))
    .appendTo("#line7");       
    
    
    //pie
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           type : 'p'}))
    .appendTo("#pie1");
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           type : 'p3',
                           size : '400x200'}))
    .appendTo("#p3d1"); 
    
    
    //gallery
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({
                           data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           //source
                           legend      : ['Data 1','Data 2','Data 3'],
                           axis_labels : ['2001','2002','2003'],
                        
                           //options 
                           size        : '400x250',
                           type        : 'bhs',
                           colors      : ['2c50f2','FFCC00','99CC00'],
                           
                           //bar
                           bar_width   : 50,
                           bar_spacing : 5,
                           
                           //bg
                           bg          : 'FFFFFF',
                           bg_type     : 'gradient',
                           bg_angle    : 90,
                           bg_offset   : '8c8c8c',
                           
                           //grid
                           grid        : true,
                           grid_x      : 5,
                           grid_y      : 5,
                           grid_line   : 5,
                           grid_blank  : 5
                         }))
    .appendTo("#gallery1");
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({
                           data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           
                           //source
                           legend      : ['Data 1','Data 2','Data 3'],
                           axis_labels : ['2001','2002','2003'],
                        
                           //options 
                           size        : '250x400',
                           type        : 'bvs',
                           
                           //bar
                           bar_width   : 50,
                           bar_spacing : 5,
                           
                           //bg
                           chbg          : 'FFFFFF',
                           chbg_type     : 'gradient',
                           chbg_angle    : 90,
                           chbg_offset   : '8c8c8c',
                           
                           //grid
                           grid        : true,
                           grid_x      : 5,
                           grid_y      : 5,
                           grid_line   : 5,
                           grid_blank  : 0
                           
                         }))
    .appendTo("#gallery2");
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({
                           data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           
                           //source
                           legend      : ['Data 1','Data 2','Data 3'],
                           axis_labels : ['2001','2002','2003'],
                        
                           //options 
                           size        : '250x400',
                           type        : 'bhg',
                           
                           //bar
                           bar_width   : 50,
                           bar_spacing : 5,
                           
                           //bg
                           chbg          : '000000',
                           chbg_trasparency: 20
                           
                         }))
    .appendTo("#gallery3");
    

    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({
                           data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           
                           //source
                           legend      : ['Data 1','Data 2','Data 3'],
                           axis_labels : ['2001','2002','2003'],
                        
                           //options 
                           size        : '400x250',
                           type        : 'bvg',
                           colors      : ['4b9b41','81419b','41599b'],
                           //bar
                           bar_width   : 20,
                           bar_spacing : 5,
                           
                           //style
                           bg          : 'ffffff',
                           bg_type     : 'stripes',
                           bg_angle    : 90,
                           bg_offset   : '999999',
                           bg_width    : 20
                         }))
    .appendTo("#gallery4");

    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({
                           data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           
                           //source
                           legend      : ['Data 1','Data 2','Data 3'],
                           axis_labels : ['2001','2002','2003'],
                        
                           //options 
                           size        : '400x250',
                           type        : 'bvg',
                           
                           //bar
                           bar_width   : 10,
                           bar_spacing : 10,
                           
                           //style
                           chbg          : 'ffffff',
                           chbg_type     : 'stripes',
                           chbg_angle    : 90,
                           chbg_offset   : '999999',
                           chbg_width    : 20
                         }))
    .appendTo("#gallery5");
    
    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({
                           data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           
                           //source
                           legend      : ['Data 1','Data 2','Data 3'],
                           axis_labels : ['2001','2002','2003'],
                        
                           //options 
                           size        : '400x250',
                           type        : 'lc',
                           
                           //style
                           chbg          : 'ffffff',
                           chbg_type     : 'stripes',
                           chbg_angle    : 90,
                           chbg_offset   : '999999',
                           chbg_width    : 20,
                           
                           bg            : 'ffffff',
                           bg_type       : 'gradient',
                           bg_angle      : 45,
                           bg_offset     : '4b9b41',
                           bg_width      : 20
                           
                         }))
    .appendTo("#gallery6");

    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({
                           data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                                     
                           //source
                           //legend      : ['Data 1','Data 2','Data 3'],
                           axis_labels : ['2001','2002','2003'],
                        
                           //options 
                           size        : '400x250',
                           type        : 'lc',
                           
                           //style
                           fillarea    : true,
                           fillbottom  : true,
                           filltop     : true,
                            
                           //series line 
                           lines       : [[3,3,3],[4,4,4],[5,5,5]],
                           
                           bg            : 'ffffff',
                           bg_type       : 'gradient',
                           bg_angle      : 45,
                           bg_offset     : '81419b',
                           bg_width      : 20
                           
                         }))
    .appendTo("#gallery7");        


    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({
                           data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           //source
                           legend      : ['Data 1','Data 2','Data 3'],
                           axis_labels : ['2001','2002','2003'],
                        
                           //options 
                           size        : '400x250',
                           type        : 'p',
                           
                           bg            : '999999',
                           bg_type       : 'gradient',
                           bg_angle      : 45,
                           bg_offset     : 'ffffff',
                           bg_width      : 20
                         }))
    .appendTo("#gallery8");    
    

    var api = new jGCharts.Api();
    jQuery('<img>')
    .attr('src', api.make({
                           data : [[153, 60, 52], [113, 70, 60], [120, 80, 40]], 
                           //source
                           legend      : ['Data 1','Data 2','Data 3'],
                           axis_labels : ['2001','2002','2003'],
                        
                           //options 
                           size        : '400x200',
                           type        : 'p3',
                           
                           bg            : 'ffffff',
                           bg_type       : 'gradient',
                           bg_angle      : 45,
                           bg_offset     : '999999',
                           bg_width      : 20
                         }))
    .appendTo("#gallery9");  
    
    jQuery(".jgtable").jgtable();   
    
    jQuery.getJSON(
        "../json/example.json",
        //wait json data
        function(json){
            jQuery("#jgjson").jgtable(json);
        }
    );
    
    jQuery.getJSON(
        "../json/delpiero.json",
        //wait json data
        function(json){
            var data_custom = [];
            for(var x= 0;x< jQuery("#jgcustomdata").find("tbody > tr").size();x++){
                data_custom.push(jQuery.map( jQuery("#jgcustomdata").find("tbody > tr:eq(" + x + ") > td"),
                   function(td,index){
                        if(index % 11 == 1 || index % 11 == 2){
                            if(parseInt(jQuery(td).text()))
                                return parseInt(jQuery(td).text());
                            else
                                return 0;
                        } 
                   }
                ));
                
            }
            var axis_custom = jQuery.map( jQuery("#jgcustomdata").find("tbody > tr > th.custom"),
                       function(th) { return jQuery(th).text(); }
            );
            var legend_custom = ["presenze", "gol"];
            
            json.axis_labels = axis_custom;
            json.data = data_custom;
            json.legend = legend_custom;
            jQuery("#jgcustomdata").jgtable(json);
            
            var data_custom = [];
            for(var x= 0;x< jQuery("#jgcustomdata").find("tbody > tr").size();x++){
                data_custom.push(jQuery.map( jQuery("#jgcustomdata").find("tbody > tr:eq(" + x + ") > td"),
                   function(td,index){
                        if(index % 11 == 9 || index % 11 == 10){
                            if(parseInt(jQuery(td).text()))
                                return parseInt(jQuery(td).text());
                            else
                                return 0;
                        } 
                   }
                ));
                
            }
            json.data = data_custom;
            json.title = 'Gol e presenze totali';
            
            jQuery("#jgcustomdata").jgtable(json);
            
     });
	
     //jQuery.ajaxHistory.initialize();                        
});