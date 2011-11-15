//
// Copyright 2009 Clemson University
//

if (Drupal.jsEnabled) {

   $(document).ready(function(){

		// Move to the result top
		var result_top = document.getElementById('tripal_search_unigene-result-top');
		if (result_top) {	
			 var target_offset = $("#tripal_search_unigene-result-top").offset();
			 var target_top = target_offset.top;
			 if (navigator.userAgent.indexOf('MSIE 8.0') !=-1) 	{
				target_top -= 60;	
			}
			$('html, body').animate({scrollTop: target_top}, 1000);
		}
   });
}
