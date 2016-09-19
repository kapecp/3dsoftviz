(function () {
    angular
        .module('app', ['ui.router', 'chart.js'])
        .config(config);

    function config($stateProvider, $urlRouterProvider) {

        // Default state
        $urlRouterProvider.otherwise("/single");

        // Setup states
        $stateProvider
            .state('single', {
                url: "/single",
                templateUrl: "views/single.html",
                controller: 'SingleController'
            })
            .state('single2', {
                url: "/single2",
                templateUrl: "views/single2.html",
                controller: 'Single2Controller'
            })
            .state('multi', {
                url: "/multi",
                templateUrl: "views/multi.html",
                controller: 'MultiController'
            })
			.state('git', {
				url: "/git",
				templateUrl: "views/git.html",
				controller: 'GitController'
			})
    }
})();