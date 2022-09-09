  <template>
    <div class="shadow">
      <div class="modalwin">
        <svg class="modalwin-content">
          <circle
            :cx="circlePositions[index] && circlePositions[index].x"
            :cy="circlePositions[index] && circlePositions[index].y"
            :r="item.radius"
            :fill="item.color"
            v-for="(item, index) in circles"
            :key="index"/>
        </svg>
      </div>
    </div>
  </template>

  <script>
  const CENTER_X = 50;
  const CENTER_Y = 50;
  const RADIUS = 20;

  function positionOnCircle(radius, angle, center) {
    return {
      x: center.x + (radius * Math.cos(toRadians(angle))),
      y: center.y + (radius * Math.sin(toRadians(angle)))
    };
  };

  function toRadians(angle) {
    return angle * Math.PI / 180;
  };

  function calculatePositions(component) {
    let angleIncrement = 360 / component.circles.length;
    let positions = {};
    component.circles.forEach((circle, index) => {
      positions[index] = positionOnCircle(
        RADIUS,
        angleIncrement * index,
        {x: CENTER_X, y: CENTER_Y}
      )
    });
    return positions;
  }

  export default {
    data() {
      return {
        circles: [
          {color: '#E0F2F1', radius: 0},
          {color: '#B2DFDB', radius: 0},
          {color: '#80CBC4', radius: 0},
          {color: '#4DB6AC', radius: 0},
          {color: '#26A69A', radius: 0},
          {color: '#00897B', radius: 0},
          {color: '#00796B', radius: 0},
          {color: '#00695C', radius: 0},
          {color: '#004D40', radius: 0},
        ],
        counter: 0,
        interval: null,
        visible: false
      }
    },
    computed: {
      circlePositions: calculatePositions
    },
    created() {
      this.interval = setInterval(() => {
        this.counter++;
        this.circles = this.circles.map((item, index) => ({
          ...item,
          radius: (this.counter + index) % 8
        }));
      }, 70);
    },
    unmounted() {
      clearInterval(this.interval);
    }
  }
  </script>

  <style scoped>
  .wait-window {
    height: 100px;
    width: 100px;
  }
  .modalwin {
    height: 100px;
    width: 100px;
    background: #719ECE;
    top: 40%; /* отступ сверху */
    right: 0;
    left: 0;
    margin: 0 auto;
    z-index:2; /* поверх всех */
    /*display: none;   сначала невидим */
    position: fixed; /* фиксированное позиционирование, окно стабильно при прокрутке */
    padding: 15px;
    border: 3px outset #000;
    border-radius: .28571429rem .28571429rem .28571429rem .28571429rem;
  }
  .modalwin .modalwin-content{
    margin-top: -15px;
    margin-left: -15px;
  }
  .shadow {
    position: fixed;
    width:100%;
    height:100%;
    z-index:1; /* поверх всех  кроме окна*/
    background:#000;
    opacity: 0.5; /*прозрачность*/
    left:0;
    top:0;
  }
  </style>
