const GENRE_NAMES = {
	28: 'Action',
	12: 'Adventure',
	16: 'Animation',
	35: 'Comedy',
	80: 'Crime',
	99: 'Documentary',
	18: 'Drama',
	10751: 'Family',
	14: 'Fantasy',
	36: 'History',
	27: 'Horror',
	10402: 'Music',
	9648: 'Mystery',
	10749: 'Romance',
	878: 'Science Fiction',
	10770: 'TV Movie',
	53: 'Thriller',
	10752: 'War',
	37: 'Western',
};

let pageCount = 1;
let maxPage = 1;

document.addEventListener('DOMContentLoaded', function () {
	let releaseSlider = document.getElementById('release-year');
	let releaseOutput = document.getElementById('year');
	releaseOutput.value = releaseSlider.value;
	releaseSlider.oninput = function () {
		releaseOutput.value = this.value;
	};
	releaseOutput.onchange = function () {
		this.value = Math.min(Math.max(this.value, this.min), this.max);
		releaseSlider.value = this.value;
	};

	let popularitySlider = document.getElementById('popularity-score');
	let popularityOutput = document.getElementById('popularity');
	popularityOutput.value = popularitySlider.value;
	popularitySlider.oninput = function () {
		popularityOutput.value = this.value;
	};
	popularityOutput.onchange = function () {
		this.value = Math.min(Math.max(this.value, this.min), this.max);
		popularitySlider.value = this.value;
	};

	let ratingSlider = document.getElementById('user-rating');
	let ratingOutput = document.getElementById('rating');
	ratingOutput.value = ratingSlider.value;
	ratingSlider.oninput = function () {
		ratingOutput.value = this.value;
	};
	ratingOutput.onchange = function () {
		this.value = Math.min(Math.max(this.value, this.min), this.max);
		ratingSlider.value = this.value;
	};

	let votesSlider = document.getElementById('user-votes');
	let votesOutput = document.getElementById('votes');
	votesOutput.value = votesSlider.value;
	votesSlider.oninput = function () {
		votesOutput.value = this.value;
	};
	votesOutput.onchange = function () {
		this.value = Math.min(Math.max(this.value, this.min), this.max);
		votesSlider.value = this.value;
	};

	document.getElementById('submitBtn').onclick = submit;
	document.getElementById('backBtn').onclick = back;
	document.getElementById('nextBtn').onclick = () => pageChange('next');
	document.getElementById('prevBtn').onclick = () => pageChange('prev');
});

function submit() {
	document.getElementsByClassName('loading-screen')[0].classList.remove('hidden');
	pageCount = 1;

	fetch('/submit', {
		method: 'POST',
		headers: { 'Content-Type': 'application/json' },
		body: JSON.stringify({
			genres: [...document.querySelectorAll('#checkboxes input:checked')].map((checkbox) => Number(checkbox.value)),
			year: parseInt(document.getElementById('year').value),
			popularity: parseFloat(document.getElementById('popularity').value),
			rating: parseFloat(document.getElementById('rating').value),
			votes: parseInt(document.getElementById('votes').value),
		}),
	})
		.then((response) => response.json())
		.then((data) => {
			document.getElementById('front-page').classList.add('hidden');
			document.getElementById('results-page').classList.remove('hidden');
			document.getElementsByClassName('loading-screen')[0].classList.add('hidden');
			const container = document.getElementById('resultsContainer');
			container.innerHTML = '';

			for (let movie of data.movies) {
				let div = document.createElement('div');
				div.className = 'movie-box';

				div.innerHTML = '';
				if (movie.poster_path) {
					div.innerHTML += `<img src="https://media.themoviedb.org/t/p/w300_and_h450_face${movie.poster_path}">`;
				}

				let genreItems = '';
				if (movie.genres.length > 0) {
					genreItems = movie.genres.map((id) => `<span class="genre-item">${GENRE_NAMES[id]}</span>`).join('');
				}

				div.innerHTML += `<div>
														<span class="movie-title">${movie.title}</span>
														<span class="movie-release-year">(${movie.release_year})</span>
													</div>
													${genreItems ? `<div class="movie-genres">${genreItems}</div>` : ''}

													<span class="movie-similarity">Similarity: ${(movie.similarity * 100).toFixed(2)}%</span>

													<br><br>`;
				div.innerHTML += `<p>${movie.overview}</p>`;

				container.appendChild(div);
			}

			document.getElementById('mergeSortTime').textContent = `${data.merge_sort_time_ms.toFixed(2)} ms`;
			document.getElementById('heapSortTime').textContent = `${data.heap_sort_time_ms.toFixed(2)} ms`;

			maxPage = Math.ceil(data.total / 25);
			updatePageIndicator();
		});
}

function updatePageIndicator() {
	document.getElementById('pageIndicator').textContent = `Page ${pageCount} / ${maxPage}`;
}

function back() {
	document.getElementById('results-page').classList.add('hidden');
	document.getElementById('front-page').classList.remove('hidden');
}

function pageChange(action) {
	if (action === 'prev') {
		if (pageCount >= 2) {
			pageCount -= 1;
		}
	}

	if (action === 'next') {
		if (pageCount < maxPage) {
			pageCount += 1;
		}
	}

	fetch('/page-change', {
		method: 'POST',
		headers: { 'Content-Type': 'application/json' },
		body: JSON.stringify({
			page: pageCount,
		}),
	})
		.then((response) => response.json())
		.then((data) => {
			document.getElementById('front-page').classList.add('hidden');
			document.getElementById('results-page').classList.remove('hidden');
			document.getElementsByClassName('loading-screen')[0].classList.add('hidden');
			const container = document.getElementById('resultsContainer');
			container.innerHTML = '';

			for (let movie of data.movies) {
				let div = document.createElement('div');
				div.className = 'movie-box';

				div.innerHTML = '';
				if (movie.poster_path) {
					div.innerHTML += `<img src="https://media.themoviedb.org/t/p/w300_and_h450_face${movie.poster_path}">`;
				}

				let genreItems = '';
				if (movie.genres.length > 0) {
					genreItems = movie.genres.map((id) => `<span class="genre-item">${GENRE_NAMES[id]}</span>`).join('');
				}

				div.innerHTML += `<div>
														<span class="movie-title">${movie.title}</span>
														<span class="movie-release-year">(${movie.release_year})</span>
													</div>
													${genreItems ? `<div class="movie-genres">${genreItems}</div>` : ''}

													<span class="movie-similarity">Similarity: ${(movie.similarity * 100).toFixed(2)}%</span>

													<br><br>`;
				div.innerHTML += `<p>${movie.overview}</p>`;

				container.appendChild(div);
			}

			updatePageIndicator();
		});
}
