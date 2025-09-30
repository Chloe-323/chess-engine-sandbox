import { useState, useEffect, useCallback } from 'react';
import useWebSocket, { ReadyState } from 'react-use-websocket';

interface ChessEngineConnectionProps {
  onMessage: (message: MessageEvent) => void;
  onStatusChange?: (status: string) => void;
}

export const ChessEngineConnection = ({ 
  onMessage,
  onStatusChange 
}: ChessEngineConnectionProps) => {
  const [socketUrl] = useState('ws://localhost:8080');
  
  const { sendMessage, lastMessage, readyState } = useWebSocket(socketUrl, {
    shouldReconnect: () => true,
    reconnectAttempts: 10,
    reconnectInterval: 3000,
  });

  // Notify parent of connection status changes
  useEffect(() => {
    onStatusChange?.(ReadyState[readyState]);
  }, [readyState, onStatusChange]);

  // Forward messages to parent
  useEffect(() => {
    if (lastMessage) {
      onMessage(lastMessage);
    }
  }, [lastMessage, onMessage]);

  // Memoize the send function
  const send = useCallback((data: any) => {
    if (readyState === ReadyState.OPEN) {
      const message = typeof data === 'string' ? data : JSON.stringify(data);
      sendMessage(message);
      return true;
    }
    return false;
  }, [sendMessage, readyState]);

  // Return the send function and connection status
  return {
    send,
    isConnected: readyState === ReadyState.OPEN,
    connectionStatus: ReadyState[readyState]
  };
};