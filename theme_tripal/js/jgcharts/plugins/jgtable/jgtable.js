/*
 * 
 * jQuery Google Charts - Table Plugin 0.9
 * 
 * $Date: 2009/10/01 18:28:41 $
 * $Rev:171 $
 * 
 * @requires
 * jGCharts Base
 * Metadata
 * 
 * Copyright (c) 2008 Massimiliano Balestrieri
 * Examples and docs at: http://maxb.net/blog/
 * Licensed GPL licenses:
 * http://www.gnu.org/licenses/gpl.html
 *
 */

if(!window.jGCharts)
    alert("Include jGCharts Base Plugin");
 
jGCharts.Table = {
    init : function(options){
        
                
        return this.each(function(nr, el){
            
            var that = this;
        	var _table = jQuery(that).find("table").eq(0);
        	
            var _options = jQuery.extend({
            	single : 'metadata'
            }, options);
            
            
            if(!_options.target){
               var _target = jQuery('<div class="jgchart">');
               jQuery(that).prepend(_target);
            }else{
               var _target = jQuery(_options.target);
            }
            
			
			_options = jQuery.extend(jQuery(that).metadata({cre: /({[\s\S]*})/, single : _options.single.toString()}), _options);
			
			//console.log(_options);
            
            if(!_options.data){
            	
            	_options.data = [];
                for(var x= 0;x< jQuery(that).find("tbody > tr").size();x++){
                    _options.data.push(
                        jQuery.map( jQuery(that).find("tbody > tr:eq(" + x + ") > td"),
                            function(td,index){
                            //if(index % 11 == 1 || index % 11 == 2){
                                if(parseFloat(jQuery(td).text()))
                                    return parseFloat(jQuery(td).text());
                                else
                                    return 0;
                            //} 
                            }
                        )
                    );
                }  
        	
            }
            //console.log(_options.data);
            if(!_options.axis_labels)
	            _options.axis_labels = jQuery.map( jQuery(that).find("tbody > tr > th.serie"),
	                               function(th) { return jQuery(th).text(); }
	            );
	        if(!_options.legend)    
	            _options.legend = jQuery.map( jQuery(that).find("thead > tr:last > th.serie"),
	                                 function(th) { return jQuery(th).text(); }
	            );
            
            var api = new jGCharts.Api();
            var url = api.make(_options);
            
            
            var ch = jQuery('<img>')
            .attr('src', url);
            
            if(_options.gui){
            	ch.addClass("jggui");
            }
            
            _target
            .append(ch);
            
        });
    }  
};
jQuery.fn.jgtable = jGCharts.Table.init;