//var COUNTER = 0;
(function($){
	function jggui(options){
		
		var _options = jQuery.extend({
			height : 440,
			width : 630,
			callback : false
		}, options);
			
		return this.each(function(){

			var that = this;
			
			//var _src = that.src;
			var _table = jQuery(that).parent().next("table").eq(0);
			var _jgtable = _table.parent();
			var _jggui = false;
			
			//FASE 1: load data
			var _axis = [];
		    var _legend = [];
			var _tables = [];
			var _palette = [];
			var _chart = false;
			var _defaults = {};
			var _icon;
			
			
			_bind();
			
			
			//BIND
			function _bind(){
				//console.log(that);
				jQuery(that).click(function(){
					_start();
					_init_gui();
				});
			}
			
			function _start(){
			
				//metadata miei
				var _cre =  /({.*})/;
				var _m = _cre.exec( _jgtable.attr("class") );
				var _my_metadata = false;
				if ( _m )
					_my_metadata = eval("(" + _m[1] + ")");;
					
			
				_load();
			
				//default colors
	    		_palette = [ "5131C9","FFCC00","DA1B1B","FF9900","FF6600","CCFFFF","CCFF00",
	        	             "CCCCCC","FF99CC","999900","999999","66FF00","66CC00","669900","660099",
	            	         "33CC00","333399","000000"];

				_chart =  _my_metadata;//jQuery.metadata.get(_jgtable);	 
			
			
				_defaults = {
					//DEVONO CORRISPONDERE
					type : 'bvg',
					height : 200,
					width : 300,
					size : "200x300",
					
					title_size : 12,	
					title_color : '000000',
					bar_width : 20,
					bar_spacing : 1,
					lines : [],
					
					axis_step : 1,
					
					bg : false,
					bg_type : false,
					bg_offset : false,
					bg_angle : 90,
					bg_width : 10,
					bg_trasparency : false,
					
					chbg : false,
					chbg_type : false,
					chbg_offset : false,
					chbg_angle : 90,
					chbg_width : 10,
					chbg_trasparency : false,
					
					grid : false,
					grid_x : 10 ,
					grid_y : 10,
					grid_line : 1,
					grid_blank : 1				
				};
			
				if(!_chart) _chart = {};
			      	
				_chart.type = _chart.type || 'bvg';
				_chart.size = _chart.size || "200x300";
				
				var _s = _chart.size.split("x");
				_chart.height = _s[0];
				_chart.width = _s[1];
				
				_chart.title_size = _chart.title_size || 12;	
				_chart.title_color = _chart.title_color || '000000';
				_chart.bar_width = _chart.bar_width || 20;
				_chart.bar_spacing = _chart.bar_spacing || 1;
				_chart.lines = _chart.lines || [];
				
				_chart.axis_step = _chart.axis_step || 1;
				
				_chart.bg = _chart.bg || false;
				_chart.bg_type = _chart.bg_type || false;
				_chart.bg_offset = _chart.bg_offset || false;
				_chart.bg_angle = _chart.bg_angle || 90;
				_chart.bg_width = _chart.bg_width || 10;
				_chart.bg_trasparency = _chart.bg_trasparency || false;
				
				_chart.chbg = _chart.chbg || false;
				_chart.chbg_type = _chart.chbg_type || false;
				_chart.chbg_offset = _chart.chbg_offset || false;
				_chart.chbg_angle = _chart.chbg_angle || 90;
				_chart.chbg_width = _chart.chbg_width || 10;
				_chart.chbg_trasparency = _chart.chbg_trasparency || false;
				
				_chart.grid = _chart.grid || false;
				_chart.grid_x = _chart.grid_x || 10 
				_chart.grid_y = _chart.grid_y || 10;
				_chart.grid_line = _chart.grid_line || 1;
				_chart.grid_blank = _chart.grid_blank || 1; 
			
				//console.log(_chart.colors);
			
				_icon = _chart.type;
				
				//console.log(this);
							
			}
			
			
			//LOAD
			function _load(){
				
		    	_axis = jQuery.map( jQuery(_table).find("tbody > tr > th.serie"),
			    	function(th) { return jQuery(th).text(); }
			    );
			    
			    for(var x= 0;x< jQuery(_table).find("tbody > tr").size();x++){ 
			        _tables.push(
			           jQuery.map( 
	    		            jQuery(_table).find("tbody > tr:eq(" + x + ") > td"), 
	                        function(td,index){ 
	                            if(parseFloat(jQuery(td).text())) 
	                                return parseFloat(jQuery(td).text()); 
	                            else 
	                                return 0;
	                        }
	                   )
	                );
	                
			    } 
			    //console.log(tables);
		    	
		    	_legend = jQuery.map( jQuery(_table).find("thead > tr:last > th.serie"),
		        	function(th) { return jQuery(th).text(); }
		    	);
		    	
			    
			}
			
			//TOGGLE
			function _toggle_filltable(){
				//show filltable
				if(_chart.fillarea){
					$("#filltable", _jggui).show();
				}else{
					$("#filltable", _jggui).hide();
				}
				
				if(_chart.filltop){
	   		    	var _color = _palette[_legend.length];
	   		    	$("#filltable", _jggui).find("#filltop").val(_color).css("background","#" + _color);
	   		    }
			}
			function _toggle_lines(){
				//console.log($(".line"));
				if(_icon == 'lc' || _chart.fillarea){
					$(".line", _jggui).show();
				}else{
					$(".line", _jggui).hide();
				}
			}
			function _toggle_axis_step(){
				//console.log(_axis.length>10)
				if(_axis.length>10){//TODO:test
					$(".axis_step", _jggui).show();
				}else{
					$(".axis_step", _jggui).hide();
				}
				//console.log($("#axis_step"));
			}
			function _toggle_bar_options(){
				//console.log(_icon)
				if(_icon == 'bvg' || _icon == 'bvs' ||
				   _icon == 'bhg' || _icon == 'bhs'
				){
					$("tr.bar", _jggui).find("th,td").show();
				}else{
					$("tr.bar", _jggui).find("th,td").hide();
				}	
			}
			function _toggle_bg(){
				//console.log(_chart.bg);
				if(_chart.bg){
					$("#background1", _jggui).val(_chart.bg);
					$(".background1", _jggui).show();

					$(".bg_trasparency", _jggui).show();
					
				}else{
					$(".background1", _jggui).hide();
					$(".bg_trasparency", _jggui).hide();
				}
				if(_chart.bg_type == "gradient" || _chart.bg_type == "stripes"){
					$("#background2", _jggui).val(_chart.bg_offset ? _chart.bg_offset : 'FFFFFF');
					$(".background2, .bg_angle", _jggui).show();
				}else{
					$(".background2, .bg_angle", _jggui).hide();
				}
				if(_chart.bg_type == "stripes"){
					$("#stripe_width", _jggui).val(_chart.bg_width);
					$(".stripe_width", _jggui).show();
				}else{
					$(".stripe_width", _jggui).hide();
				}
				
				if(_chart.chbg){
					$("#ch_background1", _jggui).val(_chart.chbg);
					$(".ch_background1", _jggui).show();

					$(".ch_bg_trasparency", _jggui).show();
					
				}else{
					$(".ch_background1", _jggui).hide();
					$(".ch_bg_trasparency", _jggui).hide();
				}
				if(_chart.chbg_type == "gradient" || _chart.chbg_type == "stripes"){
					$("#ch_background2", _jggui).val(_chart.chbg_offset ? _chart.chbg_offset : 'FFFFFF');
					$(".ch_background2, .ch_bg_angle", _jggui).show();
				}else{
					$(".ch_background2, .ch_bg_angle", _jggui).hide();
				}
				if(_chart.chbg_type == "stripes"){
					$("#ch_stripe_width", _jggui).val(_chart.chbg_width);
					$(".ch_stripe_width", _jggui).show();
				}else{
					$(".ch_stripe_width", _jggui).hide();
				}
				
			}
			function _toggle_grid(){
				//return;
				if(_chart.grid){
					$(".grid_x", _jggui).show();
					$(".grid_y", _jggui).show();
					$(".grid_line", _jggui).show();
					$(".grid_blank", _jggui).show();
				}else{
					$(".grid_x", _jggui).hide();
					$(".grid_y", _jggui).hide();
					$(".grid_line", _jggui).hide();
					$(".grid_blank", _jggui).hide();
				}
			}
			//UPDATE - REFRESH
			function _update_type(){
				_toggle_filltable();	
				_toggle_bar_options();
				_toggle_lines();
				_toggle_axis_step();
				_refresh();
			}
			function _update(chart){
				 _chart = $.extend(chart,_chart);
				 _size();
				 //###$("#JGG_top").find(".jgchart").fadeOut();//###.remove();
				 //###$("#JGG_top").append(_clone);
				 //###_clone.jgtable(_chart).hide().fadeIn();
				 if(_options.callback){
				 	_options.callback();
				 	//console.log(this);
				 	_bind();
				 }
				 var _div = _table.parent().find(".jgchart").eq(0).clone().hide();	
				 $("#JGG_top").append(_div.fadeIn());
				 //$("#JGG_top").find(".jgchart").
				 $("#export_url", _jggui).val(_div.find("img").eq(0).attr("src"));
			}
			function _export(){
				//COUNTER++;
				//var _url = $("#JGG_top").find(".jgchart > img").attr("src");
				//console.log($(_jggui).find("img"));
				var _options = _metadata();
				var _jgcc = _jgcharts(_options);
				$("#export_html", _jggui).val('');
				$("#export_metadata", _jggui).val(_options);
				//$("#export_url").val(_url);
				$("#export_jgcharts", _jggui).val(_jgcc);
			}
			function _jgcharts(opts){
				return "jQuery(TARGET_CONTAINER).jgcharts(" + opts + ")";
			}
			function _metadata(){
				var _metadata = "{\n";
				
				var _t = [];
				for(prop in _chart){
					var _x = _filter_metadata_property(prop);
					if(_x)
						_t.push(_x);
				}
				_metadata += _t.join(",\n");
				_metadata += "\n}";
				return _metadata;
			}
			function _filter_metadata_property(prop){
				switch(prop){
					case "height":
					case "width":
						return false;
					break;
					case "title_size":
					case "title_color":
						if(_chart.title)
							return _metadata_property(prop);
						else
							return false;
					break;
					case "bar_width":
					case "bar_spacing":
						if(_chart.type == "bhg"||
						   _chart.type == "bvg"||
						   _chart.type == "bhs"||
						   _chart.type == "bvs"
						){
							return _metadata_property(prop);
						}else{
							return false;
						}
					break;
					case "fillarea":
					case "fillbottom":
					case "filltop":
					case "lines":
						if(_chart.type == "lc"){
							return _metadata_property(prop);
						}else{
							return false;
						}
					break;
					case "bg":
					case "bg_type":
					case "bg_offset":
					case "bg_width":
					case "bg_angle":
					case "bg_trasparency":
						if(_chart.bg){
							return _metadata_property(prop);
						}else{
							return false;
						}
					break;
					case "chbg":
					case "chbg_type":
					case "chbg_offset":
					case "chbg_width":
					case "chbg_angle":
					case "chbg_trasparency":
						if(_chart.chbg){
							return _metadata_property(prop);
						}else{
							return false;
						}
					break;
					case "grid":
					case "grid_x":
					case "grid_y":
					case "grid_line":
					case "grid_blank":
						if(_chart.grid){
							return _metadata_property(prop);
						}else{
							return false;
						}
					break;
					default:
						return _metadata_property(prop);
					break;
				}
			}
			function _metadata_property(prop){
				var _val = _prop(_chart[prop]);
				if(_chart[prop] !== _defaults[prop] && _val)
					return "\t" + prop + " : " + _val  + "";
				else
					return false;
			}
			function _prop(prop){
				//console.log(typeof prop);
				switch(typeof prop){
					case "object":
						//console.log(prop.constructor);
						if(prop.constructor == Array){
							var _s = [];
							for(x in prop){
								_s.push(_prop(prop[x]))
							}
							_s = "[" + _s.join(",") + "]";
							if(_s !== "[]")
								return _s;
							else
								return false;
						}
					break;
					case "string":
						return "'"+ prop +"'";
					break;
					default:
						return prop;
					break;
				}
			}
			function _refresh(){
				_refresh_hex();
				_refresh_spinners();
				_export();
				_metadata_to_target();
				_update();
			}
			function _metadata_to_target(){
				var _class = _jgtable.attr("class");
				var _re = /{([\S\s]*)}/;
				if(_class)
					_class = _class.replace(_re,"");
				
				//console.log(_class);
				
				//console.log(_table.attr("class"));
				var _metadata = $("#export_metadata").val();
				
				if(_class)
					_metadata = _class + " " + _metadata;
				_jgtable.attr("class", _metadata);
			}
			function _refresh_hex(){
				_chart.colors = [];
				$(".hex").not(".custom").each(function(){
					//TODO:
					//console.log(this.id);
					if($(this).hasClass("serie")){
						_chart.colors.push(jQuery(this).val().replace("#",""));	
					}else{
						_chart[this.id] = jQuery(this).val().replace("#","");
						//console.log(_chart[this.id]);
					}
				});
				//console.log(_chart.bg)
				if(_chart.bg){
					var _j = $("#background1");
					var _val = _j.val();
					_j.css("background", "#" + _val);
					_chart.bg = _val.replace("#","");
				}
				if(_chart.bg){
					var _j = $("#background2");
					var _val = _j.val();
					_j.css("background", "#" + _val);
					_chart.bg_offset = _val.replace("#","");
				}
				if(_chart.chbg){
					var _j = $("#ch_background1");
					var _val = _j.val();
					_j.css("background", "#" + _val);
					_chart.chbg = _val.replace("#","");
				}
				if(_chart.chbg){
					var _j = $("#ch_background2");
					var _val = _j.val();
					_j.css("background", "#" + _val);
					_chart.chbg_offset = _val.replace("#","");
				}
			}
			function _refresh_spinners(){
				var x=1;var _temp = [];
				_chart.lines = [];
				$("#seriestable").find(".line_style").each(function(){
					//console.log($(this).val());
					_temp.push($(this).val());
					if(x % 3 == 0){
						_chart.lines.push(_temp);
						_temp = [];
					}
					x++;
				});
				//console.log(_chart.lines);
			}
						
			
			//helpers
			function _size(){
				_chart.size = _chart.width + 'x' + _chart.height; 
			}
			
			//INIT-EVENTS
			function _init_gui(){
				_overlay();
				_init_window();
			}
			function _init_window(){
				 $("#JGG_window").append('<div id="JGG_content"><div id="JGG_top"></div><div id="JGG_bottom"></div></div>').show();
				 //_refresh(_chart);
				 
				 _init_tabs();
			}
			function _init_tabs(){
				$("#JGG_bottom").load(_options.url+'jggui.html', function(html){
					$(this).html(html);
					_position(_options.height, _options.width);
					_jggui = $("#jggui");
					$("#jggui ul").tabs();
					
					//FASE 2.A - inizializza eventi
					
					_init_panel_colors();
					
					_init_events_panel_options();
					
					_init_events_panel_background();
					
					_init_events_panel_grid();
					
					_init_events_panel_type();
					
					_init_widgets();
					//_hex();
					
					//refresh
					$('.refresh').click(function(){
						_refresh();
					});
					
				});
			}
			function _init_widgets(){
				_init_spinner();
				_init_hex();
				_events_hex();				
				//valori predefiniti hex
			}
			function _init_spinner(){
				$(".spinner").not(".custom").spinner({max: 10, min: 0});
			}
			function _init_hex(){
				
				//colorpickers
				$('.hex').each(function(){
					var bgColor = $(this).val();
					bgColor = '#'+bgColor.replace(/#/g, "");
					$(this).val(bgColor).css('background', bgColor);
					$(this).wrap('<div class="hasPicker"></div>').after('<a href="#" class="pickerIcon"><span class="inner"></span></a>');
				});
		    	
		    	//set up colors, then remove temp pickers
				$('input.hex').each(function(){
					$('body').append('<div id="picker" style="display: none;"></div>');
					$('#picker').farbtastic(this).remove();
				});
				
			}
			
			function _init_events_panel_type(){
				
				$(".icon")
				.each(function(){
					var _img = $(this).find("img")
					var _src = _img.attr("src");
					//console.log(_src);
					//console.log(_icon);
					if(_src.indexOf(_icon + '.png') !== -1){
						$(this).addClass("pressed");
						_update_type();
					}
						
					_src = _options.url + _src;
					_img.attr('src', _src);	
				})
				.click(function(){
					$(this).parent().find(".pressed").removeClass("pressed");
					$(this).addClass("pressed");
					_icon = $(this).find("img").attr("src").replace(_options.url,"").replace("img/","").replace(".png","");
					//console.log(_type);
					switch(_icon){
						case 'lc':
						case 'bvg':
						case 'bvs':
						case 'bhg':
						case 'bhs':
						case 'p':
						case 'p3':
							_chart.type = _icon;
							_chart.fillarea = false;
							_chart.fillbottom = false;
							_chart.filltop = false;
						break;
						case 'fillall':
							_chart.type = 'lc';
							_chart.fillarea = true;
							_chart.fillbottom = true;
							_chart.filltop = true;
						break;
						case 'fillbottom':
							_chart.type = 'lc';
							_chart.fillarea = true;
							_chart.fillbottom = true;
							_chart.filltop = false;
						break;
					}
					_update_type();
					return false;
				});
				
			}
			function _init_events_panel_options(){
				$(".title").change(
					function(){
						_chart.title = $(this).val();
					}
				);
				$("#title_color").val("#" + _chart.title_color);
				
				$("#title_size")
				.val(_chart.title_size)
				.spinner({min:5, max:30})
				.bind('spinchange', function(event, ui) {
					_chart.title_size = ui.value;
				});
				
				//bug?: sul nero la size non fa effetto
				
				
				$("#ch_height")
				.val(_chart.height)
				.spinner({min:100, max:1000, stepping: 10})
				.bind('spinchange', function(event, ui) {
					_chart.height = ui.value;
				});
				
				$("#ch_width")
				.val(_chart.width)
				.spinner({min:100, max:1000, stepping: 10})
				.bind('spinchange', function(event, ui) {
					_chart.width = ui.value;
				});
				
				$("#bar_width")
				.val(_chart.bar_width)
				.spinner({min:1, max:30})
				.bind('spinchange', function(event, ui) {
					_chart.bar_width = ui.value;
				});
				$("#bar_spacing")
				.val(_chart.bar_spacing)
				.spinner({min:1, max:10})
				.bind('spinchange', function(event, ui) {
					_chart.bar_spacing = ui.value;
				});
				$("#axis_step")
				.val(_chart.axis_step)
				.spinner({min:1, max:10})
				.bind('spinchange', function(event, ui) {
					_chart.axis_step = ui.value;
				});
			}
			function _init_panel_colors(){
				_init_colors();
	   		    _toggle_lines();
			}
			function _init_colors(){
				var _tpl = '<tr><th>SERIE</th><td class="bgColor"><input type="text" name="serie" class="hex serie" value="$COLOR" /></td>';
	            _tpl += '<td class="line"><input type="text" class="spinner line_style" size="3" value="1" /></td>';
	            _tpl += '<td class="line"><input type="text" class="spinner line_style" size="3" value="0" /></td>';
	            _tpl += '<td class="line"><input type="text" class="spinner line_style" size="3" value="0" /></td>';
	   		    _tpl += '</tr>';
	   		    var _html = '';
				//console.log(_legend.length);
	   		    //console.log(_legend);
	   		    var y = 0;
	   		    for(x in _legend){
	   		    	var _color = _chart.colors && _chart.colors[y] ? _chart.colors[y] : _palette[y] || '000000';
	   		    	_html += _tpl.replace("SERIE", _legend[x]).replace("$COLOR",  _color);
	   		    	y++;
	   		    }
	   		    $("#seriestable").find("tbody").append(_html);
	   		    $("#seriestable").find(".spinner").spinner();
			}
			function _init_events_panel_background(){
				//$("#chart-background").val(_chart.bg_type)//.trigger("update");
				//$("#chartarea-background").val(_chart.bg_type)//.trigger("update");
				
				$("#chart-background").change(function(){
					var _val = $(this).val();
					if(_val == 'disabled'){
						_val = false;
						_chart.bg = _val;
						_chart.bg_offset = _val;
					}else{
						_chart.bg = 'FFFFFF';
						_chart.bg_offset = 'FFFFFF';
					}
					_chart.bg_type = _val;	
					
					$(this).trigger("update");
					_refresh();
					
					
				})
				.val(_chart.bg_type)
				.bind("update",function(){
					_toggle_bg();	
				})
				.trigger("update");
				
								
				$("#chartarea-background").change(function(){
					var _val = $(this).val();
					if(_val == 'disabled'){
						_val = false;
						_chart.chbg = _val;
						_chart.chbg_offset = _val;
					}else{
						_chart.chbg = 'FFFFFF';
						_chart.chbg_offset = 'FFFFFF';
					}
					_chart.chbg_type = _val;	
					
					$(this).trigger("update");
					_refresh();
					
				})
				.val(_chart.chbg_type)
				.bind("update",function(){
					_toggle_bg();	
				})
				.trigger("update");
				
				$("#bg_trasparency_bool").change(function(){
					if($(this).attr("checked")){
						$(".bg_trasparency_bool").show();
					}else{
						$(".bg_trasparency_bool").hide();
						_chart.bg_trasparency = false;
					}
				}).trigger("change");
				
				$("#ch_bg_trasparency_bool").change(function(){
					if($(this).attr("checked")){
						$(".ch_bg_trasparency_bool").show();
					}else{
						$(".ch_bg_trasparency_bool").hide();
						_chart.chbg_trasparency = false;
					}
				}).trigger("change");
				
				$("#bg_angle")
				.val(_chart.bg_angle)
				.spinner({min:0, max:90})
				.bind('spinchange', function(event, ui) {
					_chart.bg_angle = ui.value;
				});
				$("#ch_bg_angle")
				.val(_chart.chbg_angle)
				.spinner({min:0, max:90})
				.bind('spinchange', function(event, ui) {
					_chart.chbg_angle = ui.value;
				});
				
				$("#bg_trasparency")
				.val(_chart.bg_trasparency ? _chart.bg_trasparency : 90)
				.spinner({min:10, max:90, stepping: 10})
				.bind('spinchange', function(event, ui) {
					//console.log(ui.value);
					_chart.bg_trasparency = ui.value;
				});
				$("#ch_bg_trasparency")
				.val(_chart.chbg_trasparency ? _chart.chbg_trasparency : 90)
				.spinner({min:10, max:90, stepping: 10})
				.bind('spinchange', function(event, ui) {
					//console.log(ui.value);
					_chart.chbg_trasparency = ui.value;
				});
				
				$("#stripe_width")
				.spinner({min:10, max:100, stepping: 5})
				.bind('spinchange', function(event, ui) {
					//console.log(ui.value);
					_chart.bg_width = ui.value;
				});
				$("#ch_stripe_width")
				.spinner({min:10, max:100, stepping: 5})
				.bind('spinchange', function(event, ui) {
					//console.log(ui.value);
					_chart.chbg_width = ui.value;
				});
				
			}
			function _init_events_panel_grid(){
				
				$("#grid").change(function(){
					_chart.grid = this.checked;
					$(this).trigger("update");
					_refresh();
				})
				.val(_chart.grid)
				.bind("update",function(){
					_toggle_grid();	
				})
				.trigger("update");
				
				$("#grid_x, #grid_y, #grid_line, #grid_blank")
				.spinner({min:1, max:100, stepping: 1})
				.bind('spinchange', function(event, ui) {
					//console.log(ui.value);
					_chart[this.id] = ui.value;
				})
				.val(_chart[this.id]);
				
			}
			function _events_hex(){
				//click events for color pickers
				$('.pickerIcon').click(function(){
					if($(this).next().is('#picker')){
						$('#picker').remove();
						return false;
					}
					$('#picker').remove();
					$('a.on').removeClass('on');
					$('div.texturePicker ul:visible').fadeOut(100, function(){$(this).parent().css('position', 'static');});
					$(this).toggleClass('on').parent().css('position', 'relative');
					$(this).after('<div id="picker"></div>');
					$('#picker').farbtastic($(this).prev());
					$('body').click(function(){
						//$('#picker').remove();
					});
					return false;
				});
			}


		});
	
	};
	
/* THICKBOX METHODS */	
function _position(height, width) {
  var TB_WIDTH = (width*1) + 30 || 630; //defaults to 630 if no paramaters were added to URL
  var TB_HEIGHT = (height*1) + 40 || 440; //defaults to 440 if no paramaters were added to URL
  $("#JGG_window").css({marginLeft: '-' + parseInt((TB_WIDTH / 2),10) + 'px', width: TB_WIDTH + 'px'});
  if ( !(jQuery.browser.msie && jQuery.browser.version < 7)) { // take away IE6
    $("#JGG_window").css({marginTop: '-' + parseInt((TB_HEIGHT / 2),13) + 'px'});
  }
}
function _remove() {
   $("#JGG_window").fadeOut("fast",function(){$('#JGG_window,#JGG_overlay,#JGG_HideSelect').trigger("unload").unbind().remove();});
   $("#JGG_load").remove();
   if (typeof document.body.style.maxHeight == "undefined") {//if IE 6
     $("body","html").css({height: "auto", width: "auto"});
     $("html").css("overflow","");
   }
   jQuery(document).trigger("jggui");
   return false;
}	
function _overlay(){
    if (typeof document.body.style.maxHeight === "undefined") {//if IE 6
      $("body","html").css({height: "100%", width: "100%"});
      $("html").css("overflow","hidden");
      if (document.getElementById("JGG_HideSelect") === null) {//iframe to hide select elements in ie6
        $("body").append("<iframe id='JGG_HideSelect'></iframe><div id='JGG_overlay'></div><div id='JGG_window'></div>");
        $("#JGG_overlay").click(_remove);
      }
    }else{//all others
      if(document.getElementById("JGG_overlay") === null){
        $("body").append("<div id='JGG_overlay'></div><div id='JGG_window'></div>");
        $("#JGG_overlay").click(_remove);
      }
    }
    
    if(_detectMacXFF()){
      $("#JGG_overlay").addClass("JGG_overlayMacFFBGHack");//use png overlay so hide flash
    }else{
      $("#JGG_overlay").addClass("JGG_overlayBG");//use background and opacity
    }
}

function _detectMacXFF() {
  var userAgent = navigator.userAgent.toLowerCase();
  if (userAgent.indexOf('mac') != -1 && userAgent.indexOf('firefox')!=-1) {
    return true;
  }
}
/* THICKBOX METHODS */


jQuery.fn.jggui = jggui;
		
})(jQuery);