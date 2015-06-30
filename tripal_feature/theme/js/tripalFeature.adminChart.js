Drupal.behaviors.tripalFeature_adminSummaryChart = {
  attach: function (context, settings) {

    // Set-up the dimensions for our chart canvas.
    // Note: these are adjusted below so think of these are your minimum size.
    var margin = {top: 20, right: 50, bottom: 20, left: 100},
        fullWidth = document.getElementById('tripal-feature-admin-summary').offsetWidth,
        width = fullWidth - margin.left - margin.right,
        height = 300 - margin.top - margin.bottom;

    var color = d3.scale.ordinal()
        .range(["#a6cee3","#1f78b4","#b2df8a","#33a02c","#fb9a99","#e31a1c","#fdbf6f","#ff7f00","#cab2d6","#6a3d9a","#ffff99","#b15928"]);

    var formatNum = d3.format("0,000");

    // The data was parsed and saved into tripalFeature.admin.summary
    // in the preprocess function for this template.
    if (Drupal.settings.tripalFeature.admin.summary) {

      // Determine the max number of characters in both the type name
      // and the total number of features per bar for use in width/magin adjustments.
      var maxTypeLength = 0;
      var maxTotalLength = 0;
      var numBars = Drupal.settings.tripalFeature.admin.summary.length;
      for(var i=0; i < numBars; i++){
        var element = Drupal.settings.tripalFeature.admin.summary[i];

        if(element.name.length > maxTypeLength){
          maxTypeLength = element.name.length;
        }

        if(element.total_features.length > maxTotalLength){
          maxTotalLength = element.total_features.length;
        }
      }
      // Ensure a minimum in case something goes wrong...
      if (maxTotalLength < 4) { maxTotalLength = 4; }
      if (maxTypeLength < 10) { maxTypeLength = 10; }

      // Adjust our bottom margin based on the length of type names in the data.
      // Assume 4px/character based on the slope of the label.
      xAxisHeight = maxTypeLength * 3;
      margin.bottom = xAxisHeight + 25;

      // Adjust the width of the chart based on the number or bars (types)
      // and the length of the bar totals which need to fit on the top of the bar.
      // Assume 9px/character since it's not rotated.
      if ((width + margin.left + margin.right) < (numBars * (maxTotalLength * 9))) {
        width = numBars * (maxTotalLength * 9);
      }

      // Determine the best place for the legend. Default to top since that
      // will for sure not cause conflict... even though it looks better
      // on the right ;).
      // Logic: If the difference between the max & min bar heights is greater
      // than 1/2 the chart height (max bar height) then there "should"
      // be room for the chart nested on the right.
      minBarHeight = d3.min(Drupal.settings.tripalFeature.admin.summary, function(d,i) { return d.total_features; });
      barHeightDifference = Drupal.settings.tripalFeature.admin.maxBarHeight - minBarHeight;
      if (barHeightDifference >= Drupal.settings.tripalFeature.admin.maxBarHeight/2) {
        Drupal.settings.tripalFeature.admin.legendPosition = 'right';
      }

      // Also if we need to put the legend along the top we need to
      // increase the top margin.
      if (Drupal.settings.tripalFeature.admin.legendPosition == 'top') {
        // Draw a top legend in the margin.
        var columnWidth = d3.max(Drupal.settings.tripalFeature.admin.organisms, function(d,i) {return d.length;}) * 10;
        var maxNumColumns = Math.round(width / columnWidth);
        var numRows = Math.ceil(Drupal.settings.tripalFeature.admin.organisms.length / maxNumColumns);
        var idealNumColumns = Math.round(Drupal.settings.tripalFeature.admin.organisms.length / numRows);
        var legendMargin = {
          left: (width - (idealNumColumns * columnWidth))/2,
          right: (width - (idealNumColumns * columnWidth))/2,
          bottom: 25
        };

        margin.top = margin.top + (numRows * 20) + legendMargin.bottom;
      }

      // Set-up the scales of the chart.
      var x0 = d3.scale.ordinal()
        .rangeRoundBands([0, width], .1);
      var x1 = d3.scale.ordinal();
      var y = d3.scale.linear()
        .range([height, 0]);

      // Now set-up the axis functions.
      var xAxis = d3.svg.axis()
        .scale(x0)
        .orient('bottom');
      var yAxis = d3.svg.axis()
        .scale(y)
        .orient('left')
        .ticks(10, '');

      // Create our chart canvas.
      var svg = d3.select('#tripal-feature-admin-summary-chart').append('svg')
          .attr('width', width + margin.left + margin.right)
          .attr('height', height + margin.top + margin.bottom)
        .append('g')
          .attr('transform', 'translate(' + margin.left + ',' + margin.top + ')');

      // map the data to the x & y axis' of our chart.
      data = Drupal.settings.tripalFeature.admin.summary;
      x0.domain(data.map(function(d) { return d.name; }));
      x1.domain(Drupal.settings.tripalFeature.admin.organisms).rangeRoundBands([0, x0.rangeBand()]);
      y.domain([0, Drupal.settings.tripalFeature.admin.maxBarHeight]);

      // Create the x-axis.
      var xaxis = svg.append('g')
        .attr('class', 'x axis')
        .attr('transform', 'translate(0,' + height + ')')
        .call(xAxis);

      xaxis.selectAll("text")
        .style("text-anchor", "end")
        .attr("dx", "-.8em")
        .attr("dy", ".15em")
        .attr("transform", function(d) { return "rotate(-25)"; });

      // Label the  x-axis.
      xaxis.append('g')
        .attr('class', 'axis-label')
          .attr('transform', 'translate(' + width/2 + ',' + xAxisHeight + ')')
        .append('text')
          .attr('font-size', '16px')
          .attr('dy', '.71em')
          .style('text-anchor', 'middle')
          .text('Types of Features');

      // Create the y-axis.
      var yaxis = svg.append('g')
          .attr('class', 'y axis')
          .call(yAxis);

      // Label the y-axis.
      yaxis.append('g')
        .attr('class', 'axis-label')
          .attr('transform', 'translate(-70,' + height/2 + ')')
        .append('text')
          .attr('transform', 'rotate(-90)')
          .attr('font-size', '16px')
          .attr('dy', '.71em')
          .style('text-anchor', 'middle')
          .text('Total Number of Features');

      // Add a g element to contain each set of bars (1 per type).
      var type = svg.selectAll(".type")
          .data(data)
        .enter().append("g")
          .attr("class", "g")
          .attr("transform", function(d) { return "translate(" + x0(d.name) + ",0)"; });

      // Now add the bars :)
      // Keep in mind some processing was done in the preprocess function to
      // generate the bars array based on the organisms array
      // and pre-calculated the y0 & y1 used here.
      type.selectAll("rect")
          .data(function(d) { return d.bars; })
        .enter().append("rect")
          .attr("width", x0.rangeBand())
          .attr("y", function(d) { return y(d.y1); })
          .attr("height", function(d) { return y(d.y0) - y(d.y1); })
          .style("fill", function(d) { return color(d.name); })
        .append("svg:title")
          .text(function(d) { return formatNum(d.y1 - d.y0); });

      // Add the total to the top of the bar.
      svg.selectAll("g.bar-totals")
        .data(data)
      .enter().append('g')
        .classed('bar-totals', true)
      .append("text")
        .attr("class", "bar-label")
        .attr("text-anchor", "middle")
        .attr('font-size', '10px')
        .attr("x", function(d) { return x0(d.name) + x0.rangeBand()/2; })
        .attr("y", function(d) { return y(d.total_features) -5; })
        .text(function(d) { return formatNum(d.total_features); });

      // Legend
      //---------
      // NOTE: we would prefer to have the legend overlapping the chart on the
      // right side but this could be a problem if you have a similar count
      // for all features (legend overlaps bars) or a large number of types
      // (legend gets pushed out of the viewable screen area). In these cases
      // switch to a bottom legend.
      if (Drupal.settings.tripalFeature.admin.legendPosition == 'top') {
        // Draw a bottom legend in the margin.
        var legend = svg.append('g')
          .classed('legend', true)
          .attr('transform', 'translate(' + (legendMargin.left - 20) + ',-' + margin.top + ')');

        var legendItem = legend.selectAll('g')
            .data(Drupal.settings.tripalFeature.admin.organisms.slice().reverse())
          .enter().append("g")
            .attr("class", "legend-item")
            .attr("transform", function(d,i) {
              xOff = (i % idealNumColumns) * columnWidth;
              yOff = Math.floor(i  / idealNumColumns) * 25;
              return "translate(" + xOff + "," + yOff + ")"
          });
        legendItem.append("rect")
            //.attr("x", width - 18)
            .attr("width", 18)
            .attr("height", 18)
            .style("fill", color);
        legendItem.append("text")
            .attr("x", 24)
            .attr("y", 9)
            .attr("dy", ".35em")
            .style("text-anchor", "start")
            .attr('font-style','italic')
            .text(function(d) { return d; });
      }
      else {
        // Draw a right inset legend.
        var legend = svg.append('g')
          .classed('legend', true)
          .attr('transform', 'translate(0,-' + margin.top + ')');

        var legendItem = legend.selectAll('g')
            .data(Drupal.settings.tripalFeature.admin.organisms.slice().reverse())
          .enter().append("g")
            .attr("class", "legend")
            .attr("transform", function(d, i) { return "translate(0," + i * 20 + ")"; });
        legendItem.append("rect")
            .attr("x", width - 18)
            .attr("width", 18)
            .attr("height", 18)
            .style("fill", color);
        legendItem.append("text")
            .attr("x", width - 24)
            .attr("y", 9)
            .attr("dy", ".35em")
            .style("text-anchor", "end")
            .attr('font-style','italic')
            .text(function(d) { return d; });
      }
    }
  }
};
