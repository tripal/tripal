/*
 * 
 * jQuery Google Charts plugin 0.9
 * 
 * $Date: 2009/09/02 23:28:33 $
 * $Rev: 46 $
 * 
 * @requires
 * Sugar Arrays - Dustin Diaz | http://www.dustindiaz.com
 * 
 * Copyright (c) 2008 Massimiliano Balestrieri
 * Examples and docs at: http://maxb.net/blog/
 * Licensed GPL licenses:
 * http://www.gnu.org/licenses/gpl.html
 *
 */
 
if(!window.jGCharts)
    jGCharts = {};


jGCharts.Api = function(){
    
    // --------------  PRIVATE ATTR ----------------
    var _serie = 0;
    var _per_serie = 0;
    var _max = 0;
    var _min = 0;
    var _api = "http://chart.apis.google.com/chart?"; 
    var _params = {
        type           : "cht",
        size           : "chs",
        data           : "chd",
        colors         : "chco",
        scaling        : "chds",
        axis_type      : "chxt",
        axis_range     : "chxr",
        axis_labels    : "chxl",
        legend         : "chdl",
        bar_width      : "chbh",
        background     : "chf",
        fillarea       : "chm",//TODO : marker range & marker shape
        title		   : "chtt",
        title_style	   : "chts",
        grid		   : "chg",
        line_style	   : "chls",
        agent		   : "agent"
        
    };
    // chart type flags
    var _is_stacked = false;
    var _is_horizontal = false;
    var _is_vertical = false;
    var _is_fillarea = false;
    var _is_bar = false;
    var _is_line = false;
    var _is_pie = false;
    
    //  defaults attr
    //  type  default value
    var _type = "bvg";
    //  size  default value
    var _size = "300x200";//WxH
    //  title  default value
    var _title = false;
    //  title_style default value
    var _title_style = false;
    
    //  data default value
    var _data = false;
    //  legend default value
    var _legend = false;
    
    //  axis labels default value
    var _axis_labels = [];
    var _axis_step = 1;
    //  axis type  default value
    var _axis_type = "x,y";
    
    //  background  default value
    var _bg = false;
    var _chbg = false;
    //  background offset default value
    var _bg_offset = false;
    var _chbg_offset = false;
    //  background type default value
    var _bg_type = "solid";
    var _chbg_type = "solid";
    //  background angle default value
    var _bg_angle = 90;
    var _chbg_angle = 90;
    //  background width default value
    var _bg_width = 10;
    var _chbg_width = 10;
    //  background trasparency  default value
    var _bg_trasparency = false;
    var _chbg_trasparency = false;
    //  fillarea default value
    var _fillarea = false;
    //  fillbottom default value
    var _fillbottom = false;
    //  filltop default value
    var _filltop = false;
    // Bar width and spacing [only bar] default value                     
    var _bar_width = 20;
    var _bar_spacing = 1;
    
    //grid
    var _grid = false;
    var _grid_y = 10;
    var _grid_x = 10;
    var _grid_line = 10;
    var _grid_blank = 0;
    
    //lines
    var _lines = false;
    

    
    // ------------------------------------------------
    // Chart colors    [all] chco OK
    var _palette = [ "5131C9","FFCC00","DA1B1B","FF9900","FF6600","CCFFFF","CCFF00",
                    "CCCCCC","FF99CC","999900","999999","66FF00","66CC00","669900","660099",
                    "33CC00","333399","000000"];
    
    
    var simpleEncoding = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';

    // --------------  PRIVATE METHODS ----------------
    
    // JavaScript snippet for encoding data
    // http://code.google.com/apis/chart/#encoding_data 
	function simpleEncode(valueArray,maxValue) {
		//var chartData = ['s:'];
	   var chartData = []; 
	   for (var i = 0; i < valueArray.length; i++) {
	       var currentValue = valueArray[i];
		   if (!isNaN(currentValue) && currentValue >= 0) {
		      chartData.push(simpleEncoding.charAt(Math.round((simpleEncoding.length-1) * currentValue / maxValue)));
		   }else {
		      chartData.push('_');
		   }
	   }
	   return chartData.join('');
	}
    
    
    // ------------------------------------------------
    // TYPE OPTIONS
    // Bar chart   
    // chbh (opzioni:  chbh=10 -> altezza 10)
    function _bar_options(){
		var _ret = _bar_width;
		if(_bar_spacing)
			_ret += "," + _bar_spacing;
		return _ret;
	}
    // Chart legend     [bar, line, radar, scatter, venn]              
    function _eval_legend(){
        if(!_legend.constructor == Array)
           throw new Error("Legend must be Array");
        var _ret = "";
        for(var x=0;x < _legend.length;x++)
            _ret += _legend[x] + "|";
        _ret = _rlasttrim(_ret,"|");
        //console.log("legend:" + _ret);
        return _ret; 
    }
    // Chart data
    // chd=t:<chart data string>
    // If you have more than one set of data, 
    // separate each set with a pipe character (|).
    // For example:  chd=t:10.0,58.0,95.0|30.0,8.0,63.0
    function _eval_data(){
    	//console.log(data);
        if(!_data.constructor == Array)
           throw new Error("Data must be Array");
        var _ret = '';
        
        // i dati mi arrivano per riga. gcharts li vuole per colonna!
        var _cols = []; 
        for(var x=0;x < _data.length;x++){
            if(_data[x].constructor != Array)
                _data[x] = [_data[x]];
            
            //inverto i dati    
            for(var y = 0;y < _data[x].length;y++){
                if(!_cols[y])
                    _cols[y] = [];
                _cols[y].push((_data[x][y]));
                
                // setto quante serie ci sono nei dataset
                if(_cols[y].length > _per_serie){
                	//if(_is_line)
                	   _per_serie = _cols[y].length;
                    
                    _serie = _data[x].length;    	
                }
                
            }    
            // setto quante serie ci sono nei dataset 
            // in teria potrei farlo una volta sola
            // per un qualche grafico devo farlo qui
            //if((_is_bar || _is_pie) && _data[x].length > _per_serie)
            //    _per_serie = _data[x].length;
           	
           	//console.log(_per_serie);
           	
           	// setto il max e il min
            if(_is_stacked){
                _set_max(sum(_data[x]));
                _set_min(sum(_data[x]));
            }else{
                _set_max(_data[x]);
                _set_min(_data[x]);
            }
        }
        //console.log("X: " + _per_serie);   
        //console.log("s: " + _serie);
        
        //TODO : check type
        //TODO : options
        
        // add fill top 
        if(_is_fillarea && _filltop)
            _cols = _fill_top(_cols);
        // add fill bottom 
        if(_is_fillarea && _fillbottom)
            _cols = _fill_bottom(_cols);
        
        // inverto le serie come se le aspetta google api 
        // per colonna e non per riga.
        // io me li aspetto per riga
        
        for(var y=0;y < _cols.length;y++){
        	//_ret += _cols[y].join(",") + "|";
        	_ret += simpleEncode(_cols[y],_max)  + ",";   
            //    console.log(_cols[y].join(",") + "|");
        }
        //console.log(_cols);
        //_ret = _rlasttrim(_ret,"|");
        _ret = _rlasttrim(_ret,",");
        //console.log(_ret);
        //_ret = "cefhjkqwrlgYcfgc,QSSVXXdkfZUMRTUQ,HJJMOOUbVPKDHKLH";
        return "s:" + _ret; 
    }
    function _fill_bottom(_data){
    	var _min_serie = [];
    	for(var x = 1;x <= _per_serie;x++)
            _min_serie.push(0);
        _data.push(_min_serie);
        //console.log(_min_serie)
        return _data;
    }
    function _fill_top(_data){
    	var _max_serie = [];
    	for(var x = 1;x <= _per_serie;x++)
            _max_serie.push(_max);
    	_data.unshift(_max_serie);
        return _data;
    }
    //after _data()
    function _color(){
    	//console.log(colors);
        var _ret = "";var _t = "";var i = _serie;
        
        // add color (fake serie)
        if(_is_fillarea && _filltop)
            i++;
            
        for(var x = 0; x < i; x++){
           _t = _colors[x] || _palette[x];
           _ret += _t + ",";
        }
        _ret = _rlasttrim(_ret,",");
        //console.log(_ret);
           
        return _ret;
    }
    // ------------------------------------------------
    // Data scaling    [all - no: Maps] OK chds
    // chds=<data set 1 minimum value>,<data set 1 maximum value>,<data set n minimum value>,<data set n maximum value>
    // maximum : omit to specify 100
    function _scaling(){
    	return _min + "," + _max;
    }
    // call by _data()
    function _set_max(val){
        if(val.constructor == Array)
           val.forEach(function(nr){
               //if(nr)
                   _set_max(nr);
           });
        else
           if(_max < val)
               _max = val; 
    }
    function _set_min(val){
        if(val.constructor == Array)
           val.forEach(function(nr){
               //if(nr)
                   _set_min(nr);
           });
        else
           if(_min > val)
               _min = val; 
    }
    function _axis_range(){
        return "0,"+_min+","+ _max +"|1,"+_min+"," + _max;//?
    }
    // ------------------------------------------------
    // TODO: Pie chart and Google-o-meter labels [only bar e g-o-m]
    // Multiple axis labels   [bar, line, radar, scatter]                  
    function _eval_labels(){
        var _ret = "";
        //le labels vanno invertite se il grafico è orizontale
        if(_axis_labels.length == 0 && _per_serie > 10)
            _axis_step = parseInt(_per_serie / 10);
        
        if(_is_horizontal){
	        var _temp = [];
	        for(var x= _axis_labels.length; x > 0; x--)
	           _temp.push(_axis_labels[(x - 1)]); 
	        
	        _axis_labels = _temp;
        }
        //console.log(labels);
        for(var x=0;x < _per_serie;x++){
           //console.log((x % _axis_step) == 0);
           var _val = (x % _axis_step) == 0 ? (_axis_labels[x] || x) : '' ;
           _ret +=  _val  + "|";
        }
        //console.log(_ret);
        _ret = _rlasttrim(_ret,"|");
        //_ret = "first|second";
        //console.log("axis_labels: " + o);
        
        var _a = (_is_horizontal) ? 1 : 0; 
        
        return  _a + ":|" + _ret;//  "&chxs=0,0000dd,10|3,0000dd,12,1";//TODO?"0:|gen|feb|
    }
    // ------------------------------------------------
    // Solid fill      [all - bgcolor only:  Google-o-meter | Maps]
    // chf=<bg or c or a>,s,<color>|<bg or c or a>,s,<color>
    // * <bg or c or a> is:
    //  - bg for background fill
    //  - c for chart area fill
    //  - a to apply transparency to the whole chart.
    // * <s> indicates solid fill.
    // * <color> is an RRGGBB format hexadecimal number.
    // * the pipe character (|) separates fill definitions. 
    // No pipe character is required after the second definition.
    // ------------------------------------------------
    // Linear gradient [all - bgcolor only:  pie - no: Google-o-meter | Maps]           
    // ------------------------------------------------
    // Linear stripes  [all - bgcolor only:  pie - no: Google-o-meter | Maps]          
    function _background(){//bg, ch, type, bgkey, bgval, bgo, cho,  bg_t, ch_t){

            
        var _type = _background_type(_bg_type);
        var _chtype = _background_type(_chbg_type);
        
        var _ret = "";
        
        if(_bg && _bg_trasparency)
            _bg += parseInt(_bg_trasparency);  
       
       	if(_bg && _type == "s")
        	_ret = "bg,s," + _bg;
        
        if(_bg && _type == "lg")
        	_ret = "bg,lg," + _bg_angle + ","  + _bg + ",0," + _bg_offset +",1";
        
        if(_bg && _type == "ls")
        	_ret = "bg,ls," + _bg_angle + ","  + _bg + ",0." + parseInt(_bg_width/10) + "," + _bg_offset + ",0." + parseInt(_bg_width/10) ;
        
        //chart area
        
        if(_chbg && _chbg_trasparency)
            _chbg += parseInt(_chbg_trasparency);  
        
        if(_chbg && _bg)
            _ret += '|';
       
        if(_chbg && _chtype == "s")
        	_ret += "c,s," + _chbg;
        
        
        if(_chbg && _chtype == "lg")
        	_ret += "c,lg," + _chbg_angle + ","  + _chbg + ",0," + _chbg_offset +",1";
        
        if(_chbg && _chtype == "ls")
        	_ret += "c,ls," + _chbg_angle + ","  + _chbg + ",0." + parseInt(_chbg_width/10) + "," + _chbg_offset + ",0." + parseInt(_chbg_width/10) ;
        
            	
        //console.log(_ret);
        // TODO : chbg è sempre vero?    
    	return _ret;
    }
    function _background_type(type){
    	if(type == "solid")
    		return "s";
    	if(type == "gradient")
    		return "lg";
    	if(type == "stripes")
    		return "ls";
    	
    	return type;
    }
    
    // TODO : 
    // ------------------------------------------------
    // Shape markers   [bar, line, radar, scatter]
    // ------------------------------------------------
    // Horizontal range markers  [bar, line, radar, scatter]                  
    // ------------------------------------------------
    // Vertical range markers  [bar, line, radar, scatter]                  
	
    // Fill area   [bar, line, radar]  
    // chm=b,<color>,<start line index>,<end line index>,<any value>|b,<color>,<start line index>,<end line index>,<any value>
    //    * <color> is an RRGGBB format hexadecimal number.
    //    * <start line index> is the index of the line at which the fill starts. This is determined by the order in which data sets are specified with chd. The first data set specified has an index of zero (0), the second 1, and so on.
    //    * <end line index> is the index of the line at which the fill ends. This is determined by the order in which data sets are specified with chd. The first data set specified has an index of zero (0), the second 1, and so on.
    //    * <any value> is ignored.
    // after _data()
    function _fill_area(){
        var _arr = [];var _ret = '';
        if(_is_fillarea){
	        _arr = _color(_colors, _filltop).split(",");
	        //console.log(_arr);
	        
	        _arr.forEach(function(val,index){
	        	_ret += 'b,' + val + ',' + index + ',' +(index + 1) + ',' + '0' + '|';
	        });
	        _ret = _rlasttrim(_ret,"|");    
        }
        return _ret;
    }
    // ------------------------------------------------
    // Grid lines  [bar, line, radar, scatter]                  
    function _eval_grid(){
    	var _ret = '';
    	if(_grid_x >= 0)
    		_ret += _grid_x; 	
		if(_grid_y >= 0)
    		_ret += "," + _grid_y;
		if(_grid_line >= 0)
    		_ret += "," + _grid_line;
		if(_grid_blank >= 0)
    		_ret += "," + _grid_blank;	
		
		//console.log(_ret);
		return _ret;
	}
	// ------------------------------------------------
    // Line styles [bar, line, radar]                      
    function _line_style(){
		var _ret = "";
		_lines.forEach(function(val){
			//console.log(val);
			_ret += val.join(",") + "|";
		});
		_ret = _rlasttrim(_ret,"|"); 
		//console.log(_ret);
		return _ret;
	}
    //helpers
    function _options(options){
    	if(jGCharts.Api.type.indexOf(options.type) !== -1) 
    		_type = options.type;
    	// Chart size
        // chs=<width in pixels>x<height in pixels>
        // 1000x300, 300x1000, 600x500, 500x600, 800x375, and 375x800
        // maximum 300,000 pixels	
    	if(options.size)
    		_size = options.size;
        if(options.data)
    		_data = options.data;		
    	if(options.legend)
    		_legend = options.legend;	
    	if(options.axis_labels)
    		_axis_labels = options.axis_labels;	
    	if(options.axis_step)
            _axis_step = options.axis_step; 
        if(options.colors)
    		_colors = options.colors;	
    	else
    	   	_colors = [];
    	
    	//lines
    	if(options.lines)
    		_lines = options.lines;	
    	
    			
    	// ------------------------------------------------
    	// Chart title      [bar, line, radar, scatter, venn, pie]          
    	if(options.title)
    		_title = options.title;		
    	
		if(options.title_color && options.title_size)
    		_title_style = options.title_color + "," + options.title_size;		
    	//bar
    	if(options.bar_width)
    		_bar_width = options.bar_width;		
    	if(options.bar_spacing >= 0)
    		_bar_spacing = options.bar_spacing;		
    	
    	//line 
        if(options.fillarea)
    		_fillarea = options.fillarea;		
    	if(options.fillbottom)
    		_fillbottom = options.fillbottom;		
    	if(options.filltop)
    		_filltop = options.filltop;		
    		
    	//axis - TODO?
    	if(options.axis_type)
    		_axis_type = options.axis_type;
    		
    	//style 
    	if(options.bg)
    		_bg = options.bg;
    	if(options.bg_type)
    		_bg_type = options.bg_type;
    	if(options.bg_offset)
    		_bg_offset = options.bg_offset;
    	if(options.bg_width)
    		_bg_width = options.bg_width;
    	if(options.bg_angle >= 0)
    		_bg_angle = options.bg_angle;
    	if(options.bg_trasparency)
    		_bg_trasparency = options.bg_trasparency;
    	if(options.chbg)
    		_chbg = options.chbg;
    	if(options.chbg_type)
    		_chbg_type = options.chbg_type;
    	if(options.chbg_offset)
    		_chbg_offset = options.chbg_offset;
    	if(options.chbg_width)
    		_chbg_width = options.chbg_width;
    	if(options.chbg_angle >= 0)
    		_chbg_angle = options.chbg_angle;
    	if(options.chbg_trasparency)
    		_chbg_trasparency = options.chbg_trasparency;
    		
    	//grid
    	if(options.grid){
    		_grid = options.grid;	
	    
	    	if(options.grid_x >= 0)
	    		_grid_x = options.grid_x;	
	    	if(options.grid_y >= 0)
	    		_grid_y = options.grid_y;	
	    	if(options.grid_line >= 0)
	    		_grid_line = options.grid_line;	
	    	if(options.grid_blank >= 0)
	    		_grid_blank = options.grid_blank;	
    	}
    }
    function _param(index,data, last){
        var l = last ? "" : "&"; 
        return _params[index] + "=" + data + l;
    }
    function _flags(){
        //attr chart flag type
        _is_vertical = _type.indexOf("v") !== -1;
        _is_horizontal = _type.indexOf("h") !== -1;
        _is_stacked = _type.indexOf("s") !== -1 && 
                      _type != "ls" &&
                      _type != "lc";
        
        _is_line = (_type == "ls" || _type == "lc");
        _is_pie = (_type == "p" || _type == "p3");
        _is_fillarea = _is_line && _fillarea;
        
        _is_bar = _type.indexOf("b") !== -1;
    }
    //utils
    function _rlasttrim(str, search){
        return (str.lastIndexOf(search) !== -1) ? str.substr(0, str.lastIndexOf(search)) : str;           
    }
    // --------------  PUBLIC METHODS ----------------
    return {
        /* PUBLIC ATTRS */
        /* PUBLIC METHODS */
        make    :   function(options){
        	//console.log(options);
            //var options = _defaults(options);
            var url = _api;
            
            _options(options);
            _flags();
            
            url += _param("type", _type);
            url += _param("size", _size);
            
            if(_title)
            	url += _param("title", _title);
            if(_title_style)
            	url += _param("title_style", _title_style);

            if(_is_bar){
				url += _param("bar_width", _bar_options());
            }
            
            url += _param("axis_type", _axis_type);
            if(!_is_pie && _legend.length > 0)
				url += _param("legend", _eval_legend());
            
            url += _param("data", _eval_data());
            url += _param("scaling", _scaling());
            url += _param("axis_range", _axis_range());
            url += _param("axis_labels", _eval_labels());
            url += _param("background", _background()); 
            url += _param("colors", _color());
            
            if(_is_line && _lines)
            	url += _param("line_style", _line_style());
            
            if(_grid)
            	url += _param("grid", _eval_grid());
            
            if(_is_line && _is_fillarea)
            	url += _param("fillarea", _fill_area());
            
            url += _param("agent", "jgcharts", true);	 
            //console.log(options.colors);
            return url;
        }       
        
    }   
};
// --------------  STATIC ATTRS ----------------
// Chart type
// Bar chart (bhs, bvs, bhg, bvg)   
// Line and Sparkline chart   (lc, lxy, ls) 
// TODO: ------------------------------------------
// Radar chart (r)
// Scatter plot (s)
// Venn diagram (v)
// Google-o-meter (gom)    
// Maps (t)
// cht=<line chart type>
jGCharts.Api.type = ["bhs", "bvs", "bhg", "bvg", "lc", "ls","p","p3"];//TODO: "r","v","s","t","gom","lxy"
  
// --------------  GLOBAL FUNCTIONS ----------------
//lib - from iterators.js  
// [number] -> number
function sum( arr ) { return foldl( arr, 0, function(acc,x){ return acc + x } ) }
// [a] a (a -> a) -> a
function foldl( arr, acc, folder_fn ){
  for(var i=0; i < arr.length; i++){
    acc = folder_fn( acc, arr[i] );
  }
  return acc;
}
// --------------  GLOBAL FUNCTIONS - SUGAR ARRAYS ----------------
/*
    * Sugar Arrays (c) Creative Commons 2006
    * http://creativecommons.org/licenses/by-sa/2.5/
    * Author: Dustin Diaz | http://www.dustindiaz.com
    * Reference: http://www.dustindiaz.com/basement/sugar-arrays.html
*/
Function.prototype.method = function (name, fn) {
    this.prototype[name] = fn;
    return this;
};

if ( !Array.prototype.forEach ) {
    Array.
        method(
            'forEach',
            function(fn, thisObj) {
                var scope = thisObj || window;
                for ( var i=0, j=this.length; i < j; ++i ) {
                    fn.call(scope, this[i], i, this);
                }
            }
        ).
        method(
            'every',
            function(fn, thisObj) {
                var scope = thisObj || window;
                for ( var i=0, j=this.length; i < j; ++i ) {
                    if ( !fn.call(scope, this[i], i, this) ) {
                        return false;
                    }
                }
                return true;
            }
        ).
        method(
            'some',
            function(fn, thisObj) {
                var scope = thisObj || window;
                for ( var i=0, j=this.length; i < j; ++i ) {
                    if ( fn.call(scope, this[i], i, this) ) {
                        return true;
                    }
                }
                return false;
            }
        ).
        method(
            'map',
            function(fn, thisObj) {
                var scope = thisObj || window;
                var a = [];
                for ( var i=0, j=this.length; i < j; ++i ) {
                    a.push(fn.call(scope, this[i], i, this));
                }
                return a;
            }
        ).
        method(
            'filter',
            function(fn, thisObj) {
                var scope = thisObj || window;
                var a = [];
                for ( var i=0, j=this.length; i < j; ++i ) {
                    if ( !fn.call(scope, this[i], i, this) ) {
                        continue;
                    }
                    a.push(this[i]);
                }
                return a;
            }
        ).
        method(
            'indexOf',
            function(el, start) {
                var start = start || 0;
                for ( var i=start, j=this.length; i < j; ++i ) {
                    if ( this[i] === el ) {
                        return i;
                    }
                }
                return -1;
            }
        ).
        method(
            'lastIndexOf',
            function(el, start) {
                var start = start || this.length;
                if ( start >= this.length ) {
                    start = this.length;
                }
                if ( start < 0 ) {
                     start = this.length + start;
                }
                for ( var i=start; i >= 0; --i ) {
                    if ( this[i] === el ) {
                        return i;
                    }
                }
                return -1;
            }
        );
}
  
// --------------  JQUERY PLUGIN ----------------
jGCharts.Base = {
	init : function(options){
    	options = jQuery.extend({}, options);
    	return this.each(function(){
    		
    		if(!options.data)
    		  throw new Error("No Data");
    		
    		var api = new jGCharts.Api();
    		var url = api.make(options);
    		jQuery('<img>').attr("src",url).appendTo(this);
    		jQuery('<p>' + url + '</p>').appendTo(this);
    	});
    }
};


jQuery.fn.jgcharts = jGCharts.Base.init;