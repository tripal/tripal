/*
 * Copyright (c) 2008 Massimiliano Balestrieri
 * 
 * $Date: 2009/10/01 18:09:25 $
 * $Rev: 53 $
 * @requires jQuery v1.2.6
 * 
 * Copyright (c) 2008 Massimiliano Balestrieri
 * Examples and docs at: http://maxb.net/blog/
 * Licensed GPL licenses:
 * http://www.gnu.org/licenses/gpl.html
 * 
 * MODIFIED
 * 04/03/2008
 */ 

(function() {
    var _gaJsHost = (("https:" == document.location.protocol) ? "https://ssl.": "http://www.");
    var _url = _gaJsHost + "google-analytics.com/ga.js";
    jQuery.getScript(_url);
})();

(function($){
	
	function janalytics(){ 
		
		var _that = this;
		var _analytics = _that.attr("class").split(" ").shift();
		
		//console.log(_analytics);
		var _metadata = $.metadata;
		var _host = false;
		var _debug = false;
		var _last = false;
		
	    if(_metadata){
	    	_m1 = _that.metadata();
	    	if(_m1.host)
	    		_host = _m1.host;
	    	if(_m1.debug)
	    		_debug = _m1.debug;	
	    }
	    //alert(_debug);
	    if (_host && location.toString().indexOf(_host) === -1){
	    	return;
	    }
	    if (_analytics) { 
	        var _i = false;
	        var _c = function() {
	            //alert(_gat);
	            if(_debug) 
	            	alert("tracking");
	            var pageTracker = _gat._getTracker(_analytics);
	            pageTracker._trackPageview();
	            
	            
	            $("a.tracking").each(function(){
	            	if (_metadata){
	                  var _options = jQuery(this).metadata();
	                }else{ 
	                  var _options = {label: this.href}; //console.log(_options);
	                }
	            	
	            	$(this).click(function() {
		            	
		            	//evito che vengano mandati due click consecutivi. 
		            	//history da questo problema. ad ogni click ho due invocazioni.
		            	if(_last != _options.label){
		            		_last = _options.label;
			            	//console.log(this);
			                if(_debug) 
				            	alert("tracking");
			            
			                if (_options.label) {
			                    pageTracker._trackPageview(_options.label);
			                }
		            	}
		            });
		            
	            }); 
	        };
	        var _m = function(){
	            if(_gat) {
	                clearInterval(_i);
	                _c();
	            } 
	        };
	        _i = setInterval(_m, 300);
	    }
	}
	
    $.fn.janalytics = janalytics;
    
})(jQuery);

jQuery(window).bind("load",function() {
    jQuery("#analytics").janalytics();
});