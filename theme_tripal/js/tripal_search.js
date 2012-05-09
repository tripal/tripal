//
// Copyright 2009 Clemson University
//

if (Drupal.jsEnabled) {
   
   $(document).ready(function(){
	   // Remove Download features hyperlink if no search result is returned
	   // Disable the hyperlink for sequences in the interpro box
	   if ($('.box h2').html() == 'Your search yielded no results') {
		   $("#tripal_search_link").empty();
	   }
   });
}
