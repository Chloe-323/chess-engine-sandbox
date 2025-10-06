import './App.css';
import { Chessboard } from 'react-chessboard';
import type { PieceDropHandlerArgs } from 'react-chessboard';
import { Chess } from 'chess.js';
import { useState, useRef, useCallback } from 'react';
import { ChessEngineConnection } from './ChessEngineConnection';

function App() {
  
  // create a chess game using a ref to always have access to the latest game state within closures and maintain the game state across renders
  const chessGameRef = useRef(new Chess());
  const chessGame = chessGameRef.current;

  // track the current position of the chess game in state to trigger a re-render of the chessboard
  const [chessPosition, setChessPosition] = useState(chessGame.fen());
  const [connectionStatus, setConnectionStatus] = useState('Connecting...');

  // Handle messages from the chess engine
  const handleEngineMessage = useCallback((message: MessageEvent) => {
    try {
      const data = JSON.parse(message.data);
      if (data.type === 'move' && data.move) {
        // Make the move from the engine
        const move = chessGame.move({
          from: data.move.substring(0, 2),
          to: data.move.substring(2, 4),
          promotion: data.move.length > 4 ? data.move.substring(4, 5) : undefined
        });
        
        if (move) {
          setChessPosition(chessGame.fen());
        }
      }
    } catch (error) {
      console.error('Error processing engine message:', error);
    }
  }, [chessGame]);

  // Initialize the WebSocket connection to the chess engine
  const { send: sendToEngine, isConnected } = ChessEngineConnection({
    onMessage: handleEngineMessage,
    onStatusChange: (status) => setConnectionStatus(status)
  });

  // Request a move from the chess engine
  const getEngineMove = useCallback(() => {
    if (!isConnected) {
      console.warn('Not connected to engine');
      return;
    }

    if (chessGame.isGameOver()) {
      return;
    }

    // Send the current position to the engine
    sendToEngine({
      type: 'position',
      fen: chessGame.fen(),
      moves: []
    });

    // Request a move from the engine
    sendToEngine({
      type: 'go',
      // You can add more parameters here like time controls, depth, etc.
    });
  }, [chessGame, isConnected, sendToEngine]);

  // handle piece drop
  function onPieceDrop({
    sourceSquare,
    targetSquare
  }: PieceDropHandlerArgs) {
    // type narrow targetSquare potentially being null (e.g. if dropped off board)
    if (!targetSquare) {
      return false;
    }

    // try to make the move according to chess.js logic
    try {
      chessGame.move({
        from: sourceSquare,
        to: targetSquare,
        promotion: 'q' // always promote to a queen for example simplicity
      });



      // update the position state upon successful move to trigger a re-render of the chessboard
      setChessPosition(chessGame.fen());

      // Request a move from the engine after a short delay
      setTimeout(getEngineMove, 500);

      // return true as the move was successful
      return true;
    } catch {
      // return false as the move was not successful
      return false;
    }
  }

  // set the chessboard options
  const chessboardOptions = {
    position: chessPosition,
    onPieceDrop,
    id: 'play-vs-random'
  };

  return (
    <div className="app-container">
      <div className="status" style={{ textAlign: 'center', marginBottom: '10px', fontWeight: 'bold' }}>
        Engine Status: {connectionStatus}
      </div>
      <Chessboard options={chessboardOptions} />
    </div>
  );
}
export default App
