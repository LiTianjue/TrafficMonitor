import { h } from 'vue'

const createIcon = (content) => {
  return {
    render() {
      return h('svg', {
        viewBox: '0 0 1024 1024',
        width: '1em',
        height: '1em',
        fill: 'currentColor'
      }, [
        // Background circle (Signal Housing)
        h('circle', { cx: '512', cy: '512', r: '480', fill: '#333' }),
        // The Light Content (Arrow or Circle)
        content
      ])
    }
  }
}

// Green/Red/Yellow colors are dynamic usually, but for config we use a generic "Light" color (e.g. Green or White) inside the dark housing
const lightColor = '#409EFF' // Element Plus Primary Blue for "Configured" state, or we can use Green #67C23A

export const SignalLeft = createIcon(
  h('path', { 
    d: 'M512 128l-256 256h192v448h128v-448h192z', // Up arrow path rotated? No, let's draw correct paths.
    // Arrow Left: Points West.
    // Standard Arrow Path:
    d: 'M480 256 L224 512 L480 768 L480 608 L800 608 L800 416 L480 416 Z',
    fill: '#2ecc71' // Green
  })
)

export const SignalStraight = createIcon(
  h('path', { 
    // Arrow Up
    d: 'M256 480 L512 224 L768 480 L608 480 L608 800 L416 800 L416 480 Z',
    fill: '#2ecc71'
  })
)

export const SignalRight = createIcon(
  h('path', { 
    // Arrow Right
    d: 'M544 256 L800 512 L544 768 L544 608 L224 608 L224 416 L544 416 Z',
    fill: '#2ecc71'
  })
)

export const SignalRound = createIcon(
  h('circle', { 
    cx: '512', 
    cy: '512', 
    r: '256',
    fill: '#e74c3c' // Red for round usually, or Green. Let's use Red to distinguish or Green? Round lights are usually Red/Green/Yellow. Let's stick to Green for consistency or Red for "Main". Let's use Red for Round to make it distinct.
  })
)

export const SignalUnknown = createIcon(
    h('text', {
        x: '50%',
        y: '50%',
        dy: '.35em',
        'text-anchor': 'middle',
        'font-size': '400',
        'font-weight': 'bold',
        fill: '#909399'
    }, '?')
)
